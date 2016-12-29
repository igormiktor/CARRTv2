/*
    DriveProgram.cpp - A program for a CARRT drive represented as a singly-linked list

    Copyright (c) 2016 Igor Mikolic-Torreira.  All right reserved.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



#if CARRT_INCLUDE_PROGDRIVE_IN_BUILD


#include "DriveProgram.h"

#include "ProgDriveStates.h"



namespace
{

    class DriveProgram
    {
    public:

        void init()                     { mHead = 0; }

        void create();

        void purge();

        uint8_t len();

        void addAction( BaseProgDriveState* action );

        bool isEmpty()                  { return mHead == 0; }

        BaseProgDriveState* getHead()   { return mHead; }

    private:

        BaseProgDriveState* mHead;
    };



    void DriveProgram::create()
    {
        purge();
    }


    void DriveProgram::purge()
    {
        // Walk down the list and delete all the nodes
        while ( mHead )
        {
            BaseProgDriveState* next = mHead->getNextActionInProgram();
            delete mHead;
            mHead = next;
        }
    }


    uint8_t DriveProgram::len()
    {
        uint8_t count = 0;
        BaseProgDriveState* node = mHead;

        // Walk down the list and count the nodes
        while ( node )
        {
            ++count;
            node = node->getNextActionInProgram();
        }

        return count;
    }


    void DriveProgram::addAction( BaseProgDriveState* action )
    {
        // Only add real actions...
        if ( action )
        {
            // Take a precaution
            action->setNextActionInProgram( 0 );

            if ( !mHead )
            {
                mHead = action;
            }
            else
            {
                // Find the last one
                BaseProgDriveState* addHere = mHead;
                BaseProgDriveState* next = addHere->getNextActionInProgram();
                while ( next )
                {
                    addHere = next;
                    next = addHere->getNextActionInProgram();
                }

                addHere->setNextActionInProgram( action );
            }
        }
    }



    DriveProgram    mTheProgram;

}




void DriveProgram::init()
{
    mTheProgram.init();
}


void DriveProgram::create()
{
    mTheProgram.create();
}


void DriveProgram::purge()
{
    mTheProgram.purge();
}


uint8_t DriveProgram::len()
{
    return mTheProgram.len();
}


bool DriveProgram::isEmpty()
{
    return mTheProgram.isEmpty();
}


void DriveProgram::addAction( BaseProgDriveState* action )
{
    mTheProgram.addAction( action );
}


BaseProgDriveState* DriveProgram::getProgramStart()
{
    mTheProgram.getHead();
}


#endif  // CARRT_INCLUDE_PROGDRIVE_IN_BUILD
