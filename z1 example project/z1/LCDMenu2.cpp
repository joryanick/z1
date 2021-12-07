// LCDMenu2.cpp
//
// GoodPrototyping z1 rev.C1 desktop
// (c) 2013-2018 Jory Anick, jory@goodprototyping.com
//
// This file is part of a modified menu system released into public domain

//- Autor:   Jomelo
//- ICQ:     78048329
//- License: all Free
//- Edit:    2009.09.11

#include <TVout.h>
#include <font4x6.h>
#include <video_gen.h>

#include "LCDMenu2.h"
#include "z1.h"
  
  LCDMenu2::LCDMenu2(Menu &r, uint8_t ro, uint8_t co)
  {
      rootMenu        = &r;
      curMenu         = rootMenu;
      back            = 0;
      cols            = co;
      rows            = ro;
      cursor_pos      = 0;
      layer           = 0;
      layer_save[0]   = 0;      
  }
  
  void LCDMenu2::setCursor()
  {
  char space[3];

    space[0]=32;
    space[1]=32;
    space[2]=NULL;  
    
    if(cursor_pos > curloc-scroll)
      TV.print(10, 24+(cursor_pos*8), space);
    else
      if(cursor_pos < curloc-scroll)
        TV.print(10, 24+(curloc-scroll-1)*8, space);
      
    cursor_pos = curloc-scroll;

    TV.printPGM(10, 24+(curloc-scroll)*8, CONST_MENUCARET);
  }
  
  void LCDMenu2::doScroll()
  {
      if (curloc<0)      
          curloc=0;
      else
        while (curloc>0&&!curMenu->getChild(curloc)) // only allow it to go up to Menu item (one more if back button enabled)
          curloc--;
  
      if (curloc >= (rows+scroll))
      {
          scroll++;
          display();
      }
      else
        if (curloc < (scroll))
        {
          scroll--;
          display();
        }
        else
            setCursor();
  }
    
  void LCDMenu2::goUp()
  {
      curloc-=1;
      doScroll();
  }
  
  void LCDMenu2::goDown()
  {
      curloc+=1;
      doScroll();
  }

  void LCDMenu2::goTop()
  {
      // (j)! go topmost in the menu (11.13.11)
      curloc=0;
      doScroll();    
  }
  
  void LCDMenu2::goBack()
  {
      if(layer > 0)
      {
          back = 1;         
          goMenu(*curMenu->getParent());          
      }
      else
        goTop(); // (j)! 11.13.11
  }
  
  void LCDMenu2::goEnter()
  {
      Menu *tmp;
      tmp=curMenu;
      if ((tmp=tmp->getChild(curloc)))
      {  // the child exists
          if (tmp->canEnter)
          {  // canEnter function is set
              if (tmp->canEnter(*tmp))
              {  // it wants us to enter
                  goMenu(*tmp);
              }
          }
          else {
                  // canEnter function not set, assume entry allowed
                  goMenu(*tmp);                
                  curfuncname = tmp->name;
               }
      }
      else {  
             // child did not exist  The only time this should happen is on the back Menu item, so go back
             goBack();
           }
  }

  void LCDMenu2::goSettings(Menu &m)
  {
      // go to settings
      curloc=0;
      layer = 0;
      back = 0;
      goMenu(m);
  }
  
  void LCDMenu2::goMenu(Menu &m)
  {
  uint8_t diff;
  
      curMenu=&m;  
      if(layer < 8)
      {
          scroll = 0;
          if(back == 0)
          {
              layer_save[layer] = curloc;
              layer++;
              curloc = 0;
          } else
            {
              back = 0;
  
              if(layer > 0)
              {
                  layer--;
                  curloc = layer_save[layer];
                  
                  if(curloc >= rows)
                  {
                    diff = curloc-(rows-1);
                    for(int i=0; i<diff; i++)
                        doScroll();
                  }
              }
            }
      }
  
      if(layer >= 0 && layer <5)
        funcname[layer-1] = curMenu->name;
  
      // show menu if there is a further child
      if (curMenu->ChildExists() == true) // (j)! 11.30.11
        display();
  }

  void LCDMenu2::display()
  {
  Menu * tmp;
  int i = scroll;
  int j = 0;
  int maxi=(rows+scroll);
  char space[3];
  
       // prepare UI
      TV.clear_screen();
  
      // draw menu header & line
      TV.printPGM(6, 6, MENUCONST_MENU);
      TV.draw_line(0,14, SCREENWIDTH-1, 14, WHITE); 
  
      if (tmp=curMenu->getChild(0))
      {
          do
          {
              j++;
          } while (tmp=tmp->getSibling(1));
      }
      j--;
  
      if (tmp=curMenu->getChild(i))
      {
          space[0]=32; space[1]=32; space[2]=NULL;
          do {              
              TV.print(10, 24+(i-scroll)*8, space); // NOT FLASHING
  
              TV.printPGM(18, 24+(i-scroll)*8, (const char *) tmp->name);
              i++;
          } while ((tmp=tmp->getSibling(1))&&i<maxi);
      }
      else { // no children
             goBack();
           }
      setCursor();
  }
  
  char LCDMenu2::getChar(size_t n)
  {
    return pgm_read_byte(((const prog_char*)curfuncname) + n);
  }
  
  ////////////////////////////////////////////////////////////
  
  // Menu.cpp
   void Menu::setParent(Menu &p)
  {
    parent=&p;
  }
  
  void Menu::addSibling(Menu &s,Menu &p)
  {
    if (sibling)
    {
     sibling->addSibling(s,p);
    } else {
             sibling=&s;
             sibling->setParent(p);
           }
  }

  Menu::Menu(const prog_char *n)
  {    
    name = (const __FlashStringHelper*)n;
    canEnter=NULL;
  }
  
  Menu::Menu(const prog_char *n, boolean (*c)(Menu&))
  {
    name = (const __FlashStringHelper*)n;
    canEnter=c;
  }

  void Menu::addChild(Menu &c)
  {
    if (child)
    {
     child->addSibling(c,*this);
    } else {
             child=&c;
             child->setParent(*this);
           }
  }
  
  boolean Menu::ChildExists()
  {
    if (child)
      return true;
    else
      return false;
  }
  
  Menu * Menu::getChild(int which)
  {
    if (child)
    {
     return child->getSibling(which);
    } else  {
              // this Menu item has no children
             return NULL;
            }
  }
  
  Menu * Menu::getSibling(int howfar)
  {
    if (howfar==0)
    {
     return this;
    } else if (sibling)
         {
           return sibling->getSibling(howfar-1);
         } else //Asking for a nonexistent sibling
             {
             return NULL;
             }
  }
  
  Menu * Menu::getParent()
  {
    if (parent)
     return parent;
    else // top Menu
       return this;
  }
// end of LCDMenu2.cpp
