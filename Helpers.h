#pragma once
#include "raylib.h"
#include <string>
using namespace std;

// ============================================================
//  DIMENSIONES
// ============================================================
const int SW = 1280, SH = 720;

// ============================================================
//  COLORES
// ============================================================
const Color C_BG     = {  8, 10, 18,255};
const Color C_PANEL  = { 14, 18, 30,255};
const Color C_PANEL2 = { 20, 26, 44,255};
const Color C_BORDER = { 40, 52, 90,255};
const Color C_GREEN  = { 72,199,142,255};
const Color C_BLUE   = { 90,155,255,255};
const Color C_YELLOW = {255,210, 80,255};
const Color C_RED    = {255, 80, 80,255};
const Color C_PURPLE = {170,110,255,255};
const Color C_ORANGE = {255,155, 50,255};
const Color C_CYAN   = { 80,220,220,255};
const Color C_TEXT   = {210,220,240,255};
const Color C_MUTED  = { 90,100,130,255};
const Color C_PATH   = { 32, 52, 32,255};
const Color C_T1     = { 12, 16, 24,255};
const Color C_T2     = { 15, 20, 30,255};

// ============================================================
//  FUNCIONES DE DIBUJO
// ============================================================
inline void boxR(int x,int y,int w,int h,Color bg){
    DrawRectangleRounded({(float)x,(float)y,(float)w,(float)h},0.12f,8,bg);
}
inline void boxRL(int x,int y,int w,int h,float t,Color c){
    DrawRectangleLinesEx({(float)x,(float)y,(float)w,(float)h},(int)t,c);
}
inline void titulo(const char* t,int y,Color c=C_GREEN){
    int tw=MeasureText(t,26);
    DrawText(t,SW/2-tw/2,y,26,c);
}
inline void subtitulo(const char* t,int y){
    int tw=MeasureText(t,13);
    DrawText(t,SW/2-tw/2,y,13,C_MUTED);
}
inline void header(const char* tit,const char* sub,Color col=C_GREEN){
    DrawRectangle(0,0,SW,80,C_PANEL);
    DrawLine(0,80,SW,80,C_BORDER);
    titulo(tit,18,col);
    subtitulo(sub,52);
}
inline void btnVolver(){
    DrawText("[ESC] Volver al menu",40,SH-30,12,C_MUTED);
}
inline void drawCampo(Campo& c,int x,int y,int w,const char* label,bool sel){
    DrawText(label,x,y-18,11,C_MUTED);
    Color bg = sel ? C_PANEL2 : C_PANEL;
    Color br = sel ? C_GREEN  : C_BORDER;
    DrawRectangle(x,y,w,34,bg);
    DrawRectangleLines(x,y,w,34,br);
    DrawText(c.buf,x+10,y+9,14,C_TEXT);
    if(sel&&((int)(GetTime()*2)%2==0))
        DrawText("|",x+10+MeasureText(c.buf,14),y+9,14,C_GREEN);
}
inline bool boton(int x,int y,int w,int h,const char* txt,Color col=C_GREEN){
    Vector2 mp=GetMousePosition();
    bool hover=mp.x>x&&mp.x<x+w&&mp.y>y&&mp.y<y+h;
    DrawRectangleRounded({(float)x,(float)y,(float)w,(float)h},0.15f,8,hover?C_PANEL2:C_PANEL);
    DrawRectangleLinesEx({(float)x,(float)y,(float)w,(float)h},hover?2:1,hover?col:C_BORDER);
    int tw=MeasureText(txt,14);
    DrawText(txt,x+w/2-tw/2,y+h/2-7,14,hover?col:C_MUTED);
    return hover&&IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}
inline void feedback(const string& msg,float timer,int x,int y){
    if(timer<=0) return;
    bool ok=msg.find("Error")==string::npos&&msg.find("insuf")==string::npos&&msg.find("Completa")==string::npos;
    DrawText(msg.c_str(),x,y,13,ok?C_GREEN:C_RED);
}
