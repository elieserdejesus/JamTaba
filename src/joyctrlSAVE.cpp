#include "joyctrl.h"
#include <QMessageBox>
#include <QDebug>
#include <string.h>
//Basic implementation of a test class
//JoyCtrl will allow the user to use the Jamtaba's interface with a koystick
Joystick::Joystick(int index)
{

 SDL_Init( SDL_INIT_JOYSTICK );
 /*QMessageBox msgBox;
 msgBox.setText("SDL Init = Ok !");
 msgBox.exec();
 */

qDebug() << "new Joystick" << this;


    name = "";
    numAxes = 0;
    numButtons = 0;
    numHats = 0;
    joy = NULL;
    axes.clear();
    buttons.clear();
    hats.clear();

    init(index);

}
#include <QObject>
#include <QTimer>
#include <QList>
#include <QListIterator>

bool Joystick::init(int index)
{

   if (SDL_Init(SDL_INIT_JOYSTICK) != 0)
   {
       qCritical() << "SDL_INIT_JOYSTICK FAILED";
       return false;
   }

   joy = SDL_JoystickOpen(index);
   if (!joy)
   {
       qCritical() << "SDL_JoystickOpen FAILED, index:" << index;
       return false;
   }

   name=SDL_JoystickName(joy);
   quint8 k;

   numAxes = SDL_JoystickNumAxes(joy);
   for (k = 0; k < numAxes; ++k)
       axes[k] = 0;

   numButtons = SDL_JoystickNumButtons(joy);
   for (k = 0; k < numButtons; ++k)
       buttons[k] = 0;

   numHats = SDL_JoystickNumHats(joy);
   for (k = 0; k < numHats; ++k)
      hats[k] = 0;

   //zero the input struct
   memset(&input,0,sizeof(InputJoystick));
   /*for (int i=0;i<numButtons;i++)
   {input.buttons[i]=0;}*/
   qDebug("JOYSTICK idx: %d, name: %s (axes: %d, buttons: %d, hats: %d)",
          index,
          name,
          numAxes,
          numButtons,
          numHats);

   SDL_JoystickEventState(SDL_ENABLE);//enable event signals
   return true ;
   }


void Joystick::updateInput()
{
    static SDL_Event events; // en statique car appelle plusieurs fois par seconde


       while(SDL_PollEvent(&events)) // tant qu'il y a des évènements à traiter
       {

               switch(events.type)
               {
                   case SDL_JOYBUTTONDOWN:
                       input.buttons[events.jbutton.button] = 1; // bouton appuyé : valeur du bouton : 1
                       break;

                   case SDL_JOYBUTTONUP:
                       input.buttons[events.jbutton.button] = 0; // bouton relâché : valeur du bouton : 0
                       break;

                   case SDL_JOYAXISMOTION:
                       input.axes[events.jaxis.axis] = events.jaxis.value;
                       break;

                   case SDL_JOYHATMOTION:
                       input.hat[events.jhat.hat] = events.jhat.value;
                       break;

                   case SDL_JOYBALLMOTION:
                       //input->trackballs[evenements.jball.ball]->xrel = evenements.jball.xrel;
                       //input->trackballs[evenements.jball.ball]->yrel = evenements.jball.yrel;
                       break;

                   default:
                       break;
               }

       }
 }


Joystick::~Joystick()
{
    SDL_Quit();
    qDebug("Joystick destroyed");
    /*QMessageBox msgBox;
    msgBox.setText("SDL QUIT !");
    msgBox.exec();
    */
}

