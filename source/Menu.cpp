/************************************************

    Menu for Rover Robot

************************************************/


#include "Menu.h"

#include <stdint.h>

#include <avr/pgmspace.h>

#include "Drivers/Display.h"

#include "Utils/DebuggingMacros.h"



Menu::Menu( PGM_P menuName, const MenuList* menuList, uint8_t nbrItems, StateSelector f  ) :
mGetState( f ),
mMenuName( menuName ),
mMenuList( menuList ),
mCurrentItem( 0 ),
mNbrItems( nbrItems )
{
    // Nothing else to do
}



void Menu::init()
{
    mCurrentItem = 0;
    Display::clear();
    Display::displayOn();
    char tmp[17];
    strcpy_P( tmp, mMenuName );
    Display::displayTopRow( tmp );
    displayItem();
}



void Menu::displayItem()
{
    char tmp[17];
    strcpy_P( tmp, reinterpret_cast<PGM_P>( pgm_read_word( &(mMenuList[mCurrentItem].mMenuLabel) ) ) );
    Display::displayBottomRow( tmp );
}


void Menu::hide()
{
    Display::displayOff();
}


void Menu::show()
{
    Display::displayOn();
}


void Menu::next()
{
    ++mCurrentItem;
    mCurrentItem %= mNbrItems;
    displayItem();
}


void Menu::previous()
{
    --mCurrentItem;
    mCurrentItem += mNbrItems;
    mCurrentItem %= mNbrItems;
    displayItem();
}



State* Menu::selected()
{
    uint8_t stateId = pgm_read_byte( &(mMenuList[mCurrentItem].mStateId) );
    return mGetState( stateId );
}

