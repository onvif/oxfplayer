/************************************************************************************
* Copyright (c) 2013 ONVIF.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*    * Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*    * Redistributions in binary form must reproduce the above copyright
*      notice, this list of conditions and the following disclaimer in the
*      documentation and/or other materials provided with the distribution.
*    * Neither the name of ONVIF nor the names of its contributors may be
*      used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL ONVIF BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
************************************************************************************/

#include "crosscompilation_cxx11.h"

#include <QApplication>

#include "controller.h"
int main(int argc, char *argv[])
{
    QApplication    a(argc, argv);
    
    Engine                  engine;

    PlayerWidget            player_widget;

    FullscreenPlayerWidget  fullscreen_player_widget;

    ControlsWidget          controls_widget;

    ParserWidget            parser_widget;

    VerifyerDialog          verifyer_dialog;

    MediaParser             media_parser;

    Controller controller(engine, player_widget, fullscreen_player_widget, controls_widget, parser_widget, verifyer_dialog, media_parser);

    a.setWindowIcon(QIcon(":/icon"));
    player_widget.setWindowIcon(QIcon(":/icon"));
    fullscreen_player_widget.setWindowIcon(QIcon(":/icon"));

    return a.exec();
}

#ifdef WIN32
#include <Windows.h>
int CALLBACK WinMain(HINSTANCE , HINSTANCE , LPSTR pCmdLine, int )
{
    return main(1, &pCmdLine);
}
#endif
