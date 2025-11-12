#pragma once

using uint32 = unsigned __int32;




typedef struct __CircDbNode
{
	uint32 id;
	struct __CircDbNode* prev;
	struct __CircDbNode* next;
} CircDbNode;

class CircDbLinkedList
{
public:
	CircDbLinkedList();
	~CircDbLinkedList();

public:
	// Insert
	CircDbNode* InsertAfter(CircDbNode* p, uint32 id);
	CircDbNode* InsertFront(uint32 id);
	CircDbNode* InsertRear(uint32 id);

	// Remove
	void Remove(CircDbNode* removingNode);
	void RemoveFront();
	void RemoveRear();
	void RemoveCurrent();



public:
	CircDbNode* GetFirstNode() { return _head->next; }
	bool IsEnd(CircDbNode* node) { return node == _head; }


private:
	bool IsEmpty();
	void Clear();

private:


private:
	CircDbNode* _head;
	CircDbNode* _crnt;
};