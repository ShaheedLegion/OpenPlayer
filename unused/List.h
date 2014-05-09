#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

#ifdef __APPLE__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif


class ListIterator
{
    public:
    virtual int HandleListItem(int type, int * item){ return 0; }  //return nonzero to break out of iteration sequence
};

class List
{
    struct ListItem
    {
        int * data;
        ListItem * next;

        ListItem(int * d) : data(d), next(0)
        {

        }
    };

    ListItem head;
    ListIterator * iterator;
    int length;
    public:
        List(ListIterator * iter) : head(0), iterator(iter), length(0)
        {
        }

        void InsertItem(int * data)
        {
            length++;
            if (this->head.next == 0)
            {
                this->head.data = data;
                return;
            }
            ListItem * item = &this->head;
            while (item->next != 0)
            {
                item = item->next;
            }

            item->next = new ListItem(data);
        }

        void Iterate(int type = 0)
        {
            ListItem * item = &this->head;

            if (item->next == 0)    //the start is the end of the list
            {
                this->iterator->HandleListItem(type, item->data);
            }

            while (item->next != 0)
            {
                if (this->iterator->HandleListItem(type, item->data) == 0)
                    item = item->next;
                else
                {
                    printf("Breaking out of iteration\n");
                    break;
                }
            }
            printf("Iteration ended\n");
        }
        int Length()
        {
            return length;
        }
        int * GetHeadData()
        {
            return head.data;
        }
};

#endif // LIST_H_INCLUDED
