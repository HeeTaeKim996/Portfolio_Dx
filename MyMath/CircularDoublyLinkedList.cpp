#include "pch.h"
#include "CircularDoublyLinkedList.h"

CircDbLinkedList::CircDbLinkedList()
{
	CircDbNode* dummyNode = new CircDbNode();

	_head = _crnt = dummyNode;
	dummyNode->prev = dummyNode->next = dummyNode;
}

CircDbLinkedList::~CircDbLinkedList()
{
	Clear();
	delete _head;
}

CircDbNode* CircDbLinkedList::InsertAfter(CircDbNode* p, uint32 id)
{
	CircDbNode* ptr = new CircDbNode{ id, p, p->next };
	p->next = p->next->prev = ptr;
	_crnt = ptr;

	return ptr;
}

CircDbNode* CircDbLinkedList::InsertFront(uint32 id)
{
	return InsertAfter(_head, id);
}

CircDbNode* CircDbLinkedList::InsertRear(uint32 id)
{
	return InsertAfter(_head->prev, id);
}

void CircDbLinkedList::Remove(CircDbNode* removingNode)
{
	if (_crnt == removingNode)
	{
		_crnt = removingNode->next;
	}

	removingNode->prev->next = removingNode->next;
	removingNode->next->prev = removingNode->prev;
	delete removingNode;
}

void CircDbLinkedList::RemoveFront()
{
	if (!IsEmpty())
	{
		Remove(_head->next);
	}
}

void CircDbLinkedList::RemoveRear()
{
	if (!IsEmpty())
	{
		Remove(_head->prev);
	}
}

void CircDbLinkedList::RemoveCurrent()
{
	if (_head != _crnt)
	{
		Remove(_crnt);
	}
}




bool CircDbLinkedList::IsEmpty()
{
	return _head == _head->next;
}

void CircDbLinkedList::Clear()
{
	if (IsEmpty())
		return;

	CircDbNode* ptr = _head->next;
	while (ptr != _head)
	{
		CircDbNode* next = ptr->next;
		delete ptr;
		ptr = next;
	}

	_head->next = _head->prev = _crnt = _head;
}





