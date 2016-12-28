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



#include "DriveProgram.h"

#include "ProgDriveStates.h"



namespace
{

    class DriveProgram
    {
    public:

        void init()                     { mProgram = 0; }

        void create();

        void purge();

        void addAction( BaseProgDriveState* action );

        bool isEmpty()                  { return mProgram == 0; }

        BaseProgDriveState* getHead()   { return mProgram; }

    private:

        BaseProgDriveState* mProgram;
    };



    void DriveProgram::create()
    {
        purge();
    }


    void DriveProgram::purge()
    {
        // Walk down the list and delete all the nodes
        while ( mProgram )
        {
        BaseProgDriveState* next = mProgram->getNextActionInProgram();
        delete mProgram;
        mProgram = next;
        }
    }


    void DriveProgram::addAction( BaseProgDriveState* action )
    {
        // Take a precaution
        action->setNextActionInProgram( 0 );

        if ( !mProgram )
        {
            mProgram = action;
        }
        else
        {
            // Find the last one
            BaseProgDriveState* addHere = mProgram;
            BaseProgDriveState* next = addHere->getNextActionInProgram();
            while ( next )
            {
                addHere = next;
                next = addHere->getNextActionInProgram();
            }

            addHere->setNextActionInProgram( action );
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


