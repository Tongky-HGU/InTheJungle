/*
2020 - SW Jungle
malloc lab explicit method

mail = chotjd329@gmail.com

blog = https://velog.io/@chosh

 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
  * NOTE TO STUDENTS: Before you do anything else, please
  * provide your team information in the following struct.
  ********************************************************/
team_t team = {
	/* Team name */
	"team2",
	/* First member's full name */
	"Tongky",
	/* First member's email address */
	"chotjd329@gmail.com",
	/* Second member's full name (leave blank if none) */
	"Micky",
	/* Third member's full name (leave blank if none) */
	"Johny"};

/* �ڵ� ���� [������]

1. �ĺ��� ���ؼ� ��ũ�� ���� ���� �����ϴ� �κ��̳�, CHANGE_PREV ó�� ��ũ�� ������ �ϴ� ���� ȿ�����̶�� �����߽��ϴ�.(�������̾ �����ϱ� ��) 
2. explicit�� �����ּ��� ������ �� �����ּż� �б� �����մϴ�. ���� ������׿�
3. mm_init���� ���� ����Ʈ�� ó�� �κ��� ������ְ� �����ؼ�, �������̰� �б� ��Ȳ�� ���������� ���׿�!
4. heap_listp�� PREV_FREE_BLKP �� �� Ȱ���ؼ�, ��� �ּ� ������ �����ϳ׿�. ���� ��� �� �ٸ� ������ ��������µ�, ������ �ξ� ���� ��� �����ϴ�.
5. find_fit�� �ּ��� next_fit�ε�, next_fit ����� �´� �� �� �𸣰ڳ׿� �Ф� 

����ϼ̽��ϴ�. 
* /

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

#define WSIZE 4				// [bytes] word, header, footer size
#define DSIZE 8				// [bytes] double word size
#define CHUNKSIZE (1 << 12) // [bytes] extend heap by this amount �ϳ��� �������� 4[kb]

#define MAX(x, y) ((x) > (y) ? (x) : (y)) // max �� ��ȯ

#define PACK(size, alloc) ((size) | (alloc)) // size ���� 000 ������ allocation ���θ� ������ ��Ʈ�� ��ȯ

#define GET(p) (*(unsigned int *)(p))			   // �ּҰ����� �� �о��
#define PUT(p, val) (*(unsigned int *)(p) = (val)) // �ּҰ����� �� ��

#define GET_SIZE(p) (GET(p) & ~0x7)							 // ��� ������ �о��
#define GET_ALLOC(p) (GET(p) & 0x1)							 // �Ҵ� ���θ� �о�� \
															 // bp = block pointer
#define HDRP(bp) ((char *)(bp)-WSIZE)						 // ����� �ּҰ��� ��ȯ
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) // Ǫ���� �ּҰ��� ��ȯ, ������� ����� �� ���� double word�� ����.

// blkp = block pointer
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp)-WSIZE))) // ���� ����� �ּҰ��� ��ȯ, ������� �� ������ �����ְ� word�� ����.
#define PREV_BLKP(bp) ((char *)(bp)-GET_SIZE(((char *)(bp)-DSIZE)))	  // �� ����� �ּҰ��� ��ȯ, ������� double word�� ���� �� ����� ����� �˾Ƴ���.

#define GET_P(p) ((char *)*(unsigned int *)(p))			  // �ּҰ����� �ּҰ��� �о�� ( GET �� �ᵵ ������ �������̱� ����)
#define PUT_P(p, val) (*(unsigned int *)(p) = (int)(val)) // �ּҰ��� �ּҰ��� ���� ( PUT�� �ᵵ ������ �������̱� ����)

#define NEXTRP(bp) ((char *)(bp) + WSIZE) // ���� free�� ��� word �ּ�
#define PREVRP(bp) ((char *)(bp))		  // ���� free�� ��� word �ּ�

#define NEXT_FREE_BLKP(bp) (GET_P((char *)(bp) + WSIZE)) // ���� FREE BLOCK POINTER
#define PREV_FREE_BLKP(bp) (GET_P((char *)(bp)))		 // ���� FREE BLOCK POINTER

#define CHANGE_PREV(bp, val) (PUT_P(PREVRP(bp), val)); // ����� PREV word�� �ּҰ� val�� �־���
#define CHANGE_NEXT(bp, val) (PUT_P(NEXTRP(bp), val)); // ����� NEXT word�� �ּҰ� val�� �־���

static void *extend_heap(size_t words);
static void *coalesce(void *bp);
static void *find_fit(size_t asize);
static void place(void *bp, size_t asize);

// explit�� �߰��� �Լ�
static void cut_link(void *bp);
static void push_first(void *bp);

static char *heap_listp; // heap�� ù ��° pointer-------------------------------------------------------

int mm_init(void) // �޸� ó�� �����
{
	` if ((heap_listp = mem_sbrk(6 * WSIZE)) == (void *)-1) return -1; // mem_sbrk ȣ���ؼ� 4W �޸� request�ϴ� ��, ���� �ϸ� -1 ����
	PUT(heap_listp, 0);												   // heap:0��  free ���� (Alignment padding)
	PUT(heap_listp + (1 * WSIZE), PACK(2 * DSIZE, 1));				   // heap:1��  DSIZE�� allocated ���� (PROLOGUE HEADER)
	PUT_P(heap_listp + (2 * WSIZE), NULL);							   // heap:2 previous free block pointer �� null
	PUT_P(heap_listp + (3 * WSIZE), NULL);							   // heap:3 next free block pointer �� null
	PUT(heap_listp + (4 * WSIZE), PACK(2 * DSIZE, 1));				   // heap:4��  DSIZE�� allocated ���� (PROLOGUE PUTTER)
	PUT(heap_listp + (5 * WSIZE), PACK(0, 1));						   // heap:5��  allocated ���� (EPILOGUE HEADER)
	heap_listp += (2 * WSIZE);										   // heap_lisp �����͸� �Ű���

	if (extend_heap(CHUNKSIZE / WSIZE) == NULL) // chunk size Ȯ��(������ �ִ� ����������)
		return -1;

	return 0;
}

static void *extend_heap(size_t words)
{ // ���� �Ѿ�ٸ� ���� �߰��� �޾ƿ�---------------------------------------------
	char *bp;
	size_t size;

	size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE; // ¦���� ����
	if ((long)(bp = mem_sbrk(size)) == -1)					  // �ʹ� Ŀ�� �Ҵ� �������� return -1
		return NULL;

	PUT(HDRP(bp), PACK(size, 0));		  // block header free
	PUT(FTRP(bp), PACK(size, 0));		  // block putter free
	PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); // ���ο� epiloge ���

	return coalesce(bp); // ���� �� block�� �������ٸ� ��ģ��.
}

void *mm_malloc(size_t size) // �޸��Ҵ�-----------------------------------------------------------------------
{
	size_t asize;	   // ������ size
	size_t extendsize; // ���� chunksize�� �ѱ� ������
	char *bp;

	if (size == 0) // ���� �Է¹��� ����� 0 �̸� ����
		return NULL;

	if (size <= DSIZE) // ���� �Է¹��� ����� dsize���� �۾Ƶ� �ּ� size�� ����
		asize = 2 * DSIZE;
	else
		asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE); // 8�� ���(Dsize)�� ����

	if ((bp = find_fit(asize)) != NULL)
	{ // �� free ����� �ִٸ� �־��ش�.
		place(bp, asize);
		return bp;
	}

	extendsize = MAX(asize, CHUNKSIZE);					// ���� chunksize�� �ѱ� ��������
	if ((bp = extend_heap(extendsize / WSIZE)) == NULL) // �ѱ� �����ŭ�� ���� �Ҵ����
		return NULL;

	place(bp, asize);

	return bp;
}

static void* find_fit(size_t asize) {									 // �� �ڸ��� ã�� �Լ� next fit-----------------------------------------------------
	void* bp;													

	for (bp = PREV_FREE_BLKP(heap_listp); bp != (char *)NULL; bp = PREV_FREE_BLKP(bp))
	{ // tail�� ���������� ����� free list�� Ž��
		if (asize <= GET_SIZE(HDRP(bp)))
		{ // block�� �־��� ������� fit�ϸ�
			return bp;
		}
	}
	return NULL;
}

static void place(void *bp, size_t asize)
{									   // free ��Ͽ� �־��ִ� �Լ� ---------------------------------------------------------
	size_t csize = GET_SIZE(HDRP(bp)); // ����� ����� �о��

	if ((csize - asize) >= (2 * DSIZE))
	{ // �����ϰ� �ڸ��� ������ SPLIT ���ش�.
		PUT(HDRP(bp), PACK(asize, 1));
		PUT(FTRP(bp), PACK(asize, 1));
		cut_link(bp);

		bp = NEXT_BLKP(bp);
		PUT(HDRP(bp), PACK(csize - asize, 0));
		PUT(FTRP(bp), PACK(csize - asize, 0));
		push_first(bp);
	}
	else
	{ // �� �´´ٸ� �׳� �־��ش�.
		cut_link(bp);
		PUT(HDRP(bp), PACK(csize, 1));
		PUT(FTRP(bp), PACK(csize, 1));
	}
}

void mm_free(void *bp) //��� free��Ű�� �Լ� ---------------------------------------------------------------
{
	size_t size = GET_SIZE(HDRP(bp)); // ����� ����� �о��

	PUT(HDRP(bp), PACK(size, 0)); // ����� free �Է�
	PUT(FTRP(bp), PACK(size, 0)); // Ǫ�Ϳ� free �Է�
	coalesce(bp);				  // coalesce ������
}

static void cut_link(void *bp)
{ //����� ����� ��ũ�� ��������� �Լ� ------------------------------------------------
	if (PREV_FREE_BLKP(bp) != (char *)NULL)
	{
		CHANGE_NEXT(PREV_FREE_BLKP(bp), NEXT_FREE_BLKP(bp)); // ���� free�� next�� �� ���� free��
	}
	if (NEXT_FREE_BLKP(bp) != (char *)NULL)
	{
		CHANGE_PREV(NEXT_FREE_BLKP(bp), PREV_FREE_BLKP(bp)); // ���� free�� prev�� �� �� free��
	}
}

static void push_first(void *bp)
{ // free�� ����� �Ǿ����� ������ �Լ� -------------------------------------------------

	if (PREV_FREE_BLKP(heap_listp) != (char *)NULL)
	{												 // free list�� �����Ѵٸ�
		CHANGE_NEXT(PREV_FREE_BLKP(heap_listp), bp); // �� free ��Ͽ� ��(bp)�� �����Ѵ�.
	}
	PUT_P(PREVRP(bp), PREV_FREE_BLKP(heap_listp)); // ���� ������ ������ root�� ����� �ִ� ���
	PUT_P(NEXTRP(bp), heap_listp);				   // ���� ������ root
	PUT_P(PREVRP(heap_listp), bp);				   // root�� ������ ��(bp)
}

static void *coalesce(void *bp) // ���ӵ� free ó��--------------------------------------------------------------------
{
	size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp))); // ���� ����� alloc �ΰ�
	size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp))); // ���� ����� alloc �ΰ�
	size_t size = GET_SIZE(HDRP(bp));					// ���� ����� ������

	if (prev_alloc && next_alloc)
	{ // case 1 : �� �� �� alloc
		push_first(bp);
		return bp; // �׳� ����
	}
	else if (prev_alloc && !next_alloc)
	{ // case 2 : ������ free
		cut_link(NEXT_BLKP(bp));

		size += GET_SIZE(HDRP(NEXT_BLKP(bp))); // ���� ����� ��������� ���ļ� free��Ŵ
		PUT(HDRP(bp), PACK(size, 0));
		PUT(FTRP(bp), PACK(size, 0));

		push_first(bp);
	}
	else if (!prev_alloc && next_alloc)
	{ // case 3 : ������ free
		cut_link(PREV_BLKP(bp));

		size += GET_SIZE(HDRP(PREV_BLKP(bp))); // ���� ��������� ���ļ� free��Ŵ
		PUT(FTRP(bp), PACK(size, 0));
		PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));

		push_first(PREV_BLKP(bp));

		bp = PREV_BLKP(bp);
	}
	else
	{ // case 4 : �� �� �� free
		cut_link(NEXT_BLKP(bp));
		cut_link(PREV_BLKP(bp));

		size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
		PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
		PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));

		push_first(PREV_BLKP(bp));

		bp = PREV_BLKP(bp);
	}
	return bp;
}

void *mm_realloc(void *bp, size_t size) // reallocation--------------------------------------------------------------------
{
	void *old_dp = bp;
	void *new_dp;
	size_t copySize;

	new_dp = mm_malloc(size); // �ٸ����ٰ� �ٽ� �Ҵ� �ޱ�

	if (new_dp == NULL) // �����ϸ� NULL ����
		return NULL;

	copySize = GET_SIZE(HDRP(old_dp)); // ���� ����� ������
	if (size < copySize)			   // ��û�� ����� �۴ٸ� ����������� ī��
		copySize = size;
	memcpy(new_dp, old_dp, copySize);
	mm_free(old_dp); // ���� ������� ����
	return new_dp;
}
