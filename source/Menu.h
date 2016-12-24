/************************************************

    Menu for Rover Robot

************************************************/


#ifndef Menu_h
#define Menu_h


#include <stdint.h>

#include <avr/pgmspace.h>


class State;


struct MenuItem
{
    PGM_P              mMenuLabel;
    uint8_t            mStateId;
};

typedef MenuItem   MenuList;

typedef State* (*StateSelector)( uint8_t );



class Menu
{
public:

    Menu( PGM_P menuName, const MenuList* menuList, uint8_t nbrItems, StateSelector f  );

    void init();

    // Display the current item
    void displayItem();
    void hide();
    void show();

    void next();
    void previous();

    State* selected();

private:

    StateSelector       mGetState;
    PGM_P               mMenuName;
    const MenuList*     mMenuList;
    uint8_t             mCurrentItem;
    uint8_t             mNbrItems;
};


#endif
