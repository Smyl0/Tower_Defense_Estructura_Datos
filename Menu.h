#pragma once
#include "raylib.h"
#include "Helpers.h"
#include "Estado.h"
#include "GameScreen.h"
#include "ListaTorres.h"
#include "ListaEnemigos.h"
#include "ListaOleadas.h"
#include <cstdio>
#include <cstring>
#include <vector>
using namespace std;

class Menu {
private:
    Campo fNombre,fTipo,fPos,fDanio,fRango,fCosto;
    Campo fOId,fOCant,fOTipo,fOVida,fOVel;
    Campo fDelId;
    int   campoActivo=0;
    string fbMsg="";
    float  fbTimer=0;
    int    waveIdCnt=5;
    int    oleadaSeleccionada=0; // para selector de oleada

    Texture2D texFondo={0};
    Font      fontPixel={0};
    bool      recursosListos=false;

    void resetCampos(){
        fNombre.clear();fTipo.clear();fPos.clear();fDanio.clear();fRango.clear();fCosto.clear();
        fOId.clear();fOCant.clear();fOTipo.clear();fOVida.clear();fOVel.clear();
        fDelId.clear();campoActivo=0;fbMsg="";fbTimer=0;
        // Desactivar todos
        fNombre.activo=false;fTipo.activo=false;fPos.activo=false;
        fDanio.activo=false;fRango.activo=false;fCosto.activo=false;
        fOId.activo=false;fOCant.activo=false;fOTipo.activo=false;
        fOVida.activo=false;fOVel.activo=false;fDelId.activo=false;
    }

    void drawPixelText(const char* txt,int x,int y,int sz,Color col){
        if(fontPixel.baseSize>0) DrawTextEx(fontPixel,txt,{(float)x,(float)y},(float)sz,2,col);
        else DrawText(txt,x,y,sz,col);
    }
    int measurePixelText(const char* txt,int sz){
        if(fontPixel.baseSize>0) return (int)MeasureTextEx(fontPixel,txt,(float)sz,2).x;
        return MeasureText(txt,sz);
    }

    // Botón volver visual (no usa ESC)
    bool btnVolver2(){
        int bx=20, by=SH-50, bw=160, bh=34;
        Vector2 mp=GetMousePosition();
        bool hover=mp.x>bx&&mp.x<bx+bw&&mp.y>by&&mp.y<by+bh;
        DrawRectangleRounded({(float)bx,(float)by,(float)bw,(float)bh},0.15f,8,hover?C_PANEL2:C_PANEL);
        DrawRectangleLinesEx({(float)bx,(float)by,(float)bw,(float)bh},hover?2:1,hover?C_YELLOW:C_BORDER);
        DrawText("< VOLVER",bx+16,by+10,13,hover?C_YELLOW:C_MUTED);
        return hover&&IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    }

public:
    void cargarRecursos(){
        if(recursosListos)return;
        texFondo=LoadTexture("fondo.png");
        fontPixel=LoadFont("PressStart2P-Regular.ttf");
        recursosListos=true;
    }
    void descargarRecursos(){
        if(!recursosListos)return;
        UnloadTexture(texFondo);UnloadFont(fontPixel);
    }
    void onEnter(Screen s){
        resetCampos();
        if (s == SCR_AVANZAR_TURNO) {
            turnoEjecutado = false;
            indiceTurno = 0;
        }
    }

    // ============================================================
    //  MENU PRINCIPAL
    // ============================================================
    Screen drawMenu(const Estado& estado,int nT,int oleadaIdx,
                    ListaSecuencialTorres& lT,ListaDobleEnemigos& lE,ListaCircularOleadas& lO){
        cargarRecursos();
        if(texFondo.id>0){
            float sx=(float)SW/texFondo.width,sy=(float)SH/texFondo.height;
            float sc=sx>sy?sx:sy;
            int dw=(int)(texFondo.width*sc),dh=(int)(texFondo.height*sc);
            int ox=(SW-dw)/2,oy=(SH-dh)/2;
            DrawTexturePro(texFondo,{0,0,(float)texFondo.width,(float)texFondo.height},
                {(float)ox,(float)oy,(float)dw,(float)dh},{0,0},0,WHITE);
        }
        DrawRectangle(0,0,SW,SH,{0,0,0,145});

        const char* tit="TOWER DEFENSE";
        int tw=measurePixelText(tit,34);
        drawPixelText(tit,SW/2-tw/2+3,33,34,{0,0,0,200});
        drawPixelText(tit,SW/2-tw/2,30,34,C_YELLOW);
        const char* sub="Estructura de Datos en C++";
        int sw2=measurePixelText(sub,9);
        drawPixelText(sub,SW/2-sw2/2,78,9,C_GREEN);

        struct Op{const char* txt;Screen scr;};
        Op ops[10]={
            {"1. JUGAR",            SCR_JUEGO},
            {"2. REGISTRAR TORRE",  SCR_REG_TORRE},
            {"3. VER TORRES",       SCR_SHOW_TORRES},
            {"4. ELIMINAR TORRE",   SCR_DEL_TORRE},
            {"5. REGISTRAR OLEADA", SCR_REG_OLEADA},
            {"6. VER OLEADAS",      SCR_SHOW_OLEADAS},
            {"7. INICIAR OLEADA",   SCR_INICIAR_OLEADA},
            {"8. SKIP RONDA",        SCR_AVANZAR_TURNO},
            {"9. ESTADO DEL JUEGO", SCR_ESTADO},
            {"10. SALIR",           SCR_SALIR},
        };

        int menuW=540,itemH=42,gap=7;
        int menuX=SW/2-menuW/2,menuY=108;
        Screen next=SCR_MENU;
        Vector2 mp=GetMousePosition();

        for(int i=0;i<10;i++){
            int bx=menuX,by=menuY+i*(itemH+gap);
            bool hover=mp.x>bx&&mp.x<bx+menuW&&mp.y>by&&mp.y<by+itemH;
            DrawRectangleRounded({(float)bx,(float)by,(float)menuW,(float)itemH},0.08f,8,
                hover?Color{0,25,12,220}:Color{0,10,5,185});
            DrawRectangleLinesEx({(float)bx,(float)by,(float)menuW,(float)itemH},
                hover?2:1,hover?C_GREEN:Color{0,120,60,160});
            DrawCircle(bx+22,by+itemH/2,4,hover?C_GREEN:Color{0,160,80,200});
            int tw2=measurePixelText(ops[i].txt,12);
            drawPixelText(ops[i].txt,bx+menuW/2-tw2/2,by+itemH/2-7,12,hover?C_YELLOW:(i==9?C_RED:C_GREEN));
            if(hover&&IsMouseButtonPressed(MOUSE_LEFT_BUTTON))next=ops[i].scr;
        }

        int oleadaActual = 0;
        Oleada* oActual = lO.getActual();
        if (oActual) oleadaActual = oActual->idOleada;

        char buf[90];
        sprintf(buf,"VIDAS:%d  DINERO:$%d  PUNTOS:%d  OLEADA:%d/4",estado.vidas,estado.dinero,estado.puntuacion,oleadaActual);
        int bw2=measurePixelText(buf,8);
        DrawRectangle(0,SH-36,SW,36,{0,0,0,170});
        drawPixelText(buf,SW/2-bw2/2,SH-24,8,C_GREEN);
        return next;
    }

    // ============================================================
    //  REGISTRAR TORRE
    // ============================================================
    Screen drawRegTorre(float dt,Estado& estado,ListaSecuencialTorres& lT,GameScreen& gs){
        if(fbTimer>0)fbTimer-=dt;
        header("Registrar Torre Defensiva","Lista Secuencial — Insertar al final",C_GREEN);
        if(btnVolver2()) return SCR_MENU;

        // Click en campo para activarlo
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            Vector2 mp=GetMousePosition();
            int cx=SW/2-300,cy=178;
            // Campos clickeables
            struct{int x,y,w,h;int idx;}areas[]={
                {cx,cy,280,34,0},{cx,cy+60,130,34,2},{cx+148,cy+60,130,34,3},
                {cx,cy+120,130,34,4},{cx+148,cy+120,130,34,5}
            };
            campoActivo=-1;
            for(auto&a:areas) if(mp.x>a.x&&mp.x<a.x+a.w&&mp.y>a.y&&mp.y<a.y+a.h) campoActivo=a.idx;
        }
        if(IsKeyPressed(KEY_TAB)) campoActivo=(campoActivo+1)%6;

        fNombre.activo=(campoActivo==0);fTipo.activo=(campoActivo==1);
        fPos.activo=(campoActivo==2);fDanio.activo=(campoActivo==3);
        fRango.activo=(campoActivo==4);fCosto.activo=(campoActivo==5);
        fNombre.update();fTipo.update();fPos.update();fDanio.update();fRango.update();fCosto.update();

        int cx=SW/2-300,cy=100;
        DrawText("Selecciona tipo (clic) o escribe manualmente:",cx,cy,13,C_MUTED);cy+=24;
        for(int i=0;i<N_TIPOS;i++){
            int bx=cx+i*152;
            bool sel=(string(fTipo.buf)==TIPOS[i].nom);
            DrawRectangleRounded({(float)bx,(float)cy,142,46},0.15f,8,sel?C_PANEL2:C_PANEL);
            DrawRectangleLinesEx({(float)bx,(float)cy,142,46},sel?2:1,sel?TIPOS[i].col:C_BORDER);
            DrawCircle(bx+16,cy+23,7,TIPOS[i].col);
            DrawText(TIPOS[i].nom,bx+30,cy+8,13,C_TEXT);
            char info[30];sprintf(info,"$%d D:%d R:%d",TIPOS[i].costo,TIPOS[i].danio,TIPOS[i].rango);
            DrawText(info,bx+30,cy+26,10,C_MUTED);
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                Vector2 mp=GetMousePosition();
                if(mp.x>bx&&mp.x<bx+142&&mp.y>cy&&mp.y<cy+46){
                    strcpy(fTipo.buf,TIPOS[i].nom);fTipo.len=strlen(TIPOS[i].nom);
                    char ps[8];
                    sprintf(ps,"%d",TIPOS[i].costo);strcpy(fCosto.buf,ps);fCosto.len=strlen(ps);
                    sprintf(ps,"%d",TIPOS[i].danio);strcpy(fDanio.buf,ps);fDanio.len=strlen(ps);
                    sprintf(ps,"%d",TIPOS[i].rango);strcpy(fRango.buf,ps);fRango.len=strlen(ps);
                    campoActivo=-1;
                }
            }
        }
        cy+=62;
        drawCampo(fNombre,cx,cy,280,"Nombre de la torre  [TAB o clic para cambiar campo]",campoActivo==0);cy+=60;
        drawCampo(fPos,cx,cy,130,"Posicion (0-20)",campoActivo==2);
        drawCampo(fDanio,cx+148,cy,130,"Danio",campoActivo==3);cy+=60;
        drawCampo(fRango,cx,cy,130,"Rango",campoActivo==4);
        drawCampo(fCosto,cx+148,cy,130,"Costo ($)",campoActivo==5);cy+=58;

        if(boton(cx,cy,280,44,"REGISTRAR TORRE",C_GREEN)||IsKeyPressed(KEY_ENTER)){
            if(strlen(fNombre.buf)>0&&strlen(fTipo.buf)>0&&fCosto.toInt()>0){
                Torre t;t.id=estado.idTorre;t.nombre=fNombre.buf;t.tipo=fTipo.buf;
                t.posicion=fPos.toInt();t.danio=fDanio.toInt();t.rango=fRango.toInt();t.costo=fCosto.toInt();
                if(estado.dinero>=t.costo){
                    estado.dinero-=t.costo;lT.insertar(t);
                    int col=min(max(t.posicion,0),GCOLS-1);
                    int tipo=0;for(int i=0;i<N_TIPOS;i++)if(string(TIPOS[i].nom)==t.tipo){tipo=i;break;}
                    gs.addTorreVisual(estado.idTorre++,col,5,tipo);
                    fbMsg="Torre registrada correctamente!";fbTimer=3;resetCampos();
                }else{fbMsg="Error: Dinero insuficiente ($"+to_string(estado.dinero)+" disponible)";fbTimer=3;}
            }else{fbMsg="Completa nombre, tipo y costo!";fbTimer=3;}
        }
        feedback(fbMsg,fbTimer,cx,cy+54);
        int n=lT.getTamanio();
        if(n>0){
            DrawText("Lista Secuencial actual:",cx,cy+80,12,C_MUTED);
            for(int i=0;i<min(n,5);i++){
                int bx=cx+i*138;
                DrawRectangle(bx,cy+96,126,42,C_PANEL2);DrawRectangleLines(bx,cy+96,126,42,C_BORDER);
                char lb[24];sprintf(lb,"[%d] %s",i,lT.get(i).nombre.substr(0,8).c_str());
                DrawText(lb,bx+6,cy+106,11,C_TEXT);
                DrawText(TextFormat("D:%d R:%d",lT.get(i).danio,lT.get(i).rango),bx+6,cy+122,10,C_MUTED);
                if(i<min(n,5)-1)DrawText("->",bx+128,cy+110,13,C_GREEN);
            }
        }
        return SCR_REG_TORRE;
    }

    // ============================================================
    //  VER TORRES
    // ============================================================
    Screen drawShowTorres(ListaSecuencialTorres& lT){
        header("Torres Registradas","Lista Secuencial — Recorrido completo",C_CYAN);
        if(btnVolver2())return SCR_MENU;
        int n=lT.getTamanio();
        if(n==0){subtitulo("No hay torres. Usa la opcion 2 para agregar.",370);return SCR_SHOW_TORRES;}
        int tx=80,ty=96,rh=36;
        int widths[]={50,160,120,80,70,70,80};
        const char*heads[]={"ID","Nombre","Tipo","Pos","Danio","Rango","Costo"};
        DrawRectangle(tx,ty,SW-160,rh,C_PANEL2);
        int cx=tx+8;
        for(int i=0;i<7;i++){DrawText(heads[i],cx,ty+10,12,C_MUTED);cx+=widths[i];}
        DrawLine(tx,ty+rh,tx+SW-160,ty+rh,C_BORDER);
        for(int i=0;i<n;i++){
            Torre&t=lT.get(i);int ry=ty+rh+i*rh;
            DrawRectangle(tx,ry,SW-160,rh,i%2==0?C_PANEL:C_PANEL2);
            cx=tx+8;char vs[7][32];
            sprintf(vs[0],"%d",t.id);sprintf(vs[1],"%s",t.nombre.substr(0,14).c_str());
            sprintf(vs[2],"%s",t.tipo.substr(0,10).c_str());sprintf(vs[3],"%d",t.posicion);
            sprintf(vs[4],"%d",t.danio);sprintf(vs[5],"%d",t.rango);sprintf(vs[6],"$%d",t.costo);
            Color tc=C_TEXT;for(int j=0;j<N_TIPOS;j++)if(string(TIPOS[j].nom)==t.tipo){tc=TIPOS[j].col;break;}
            for(int j=0;j<7;j++){DrawText(vs[j],cx,ry+10,12,j==1?tc:C_TEXT);cx+=widths[j];}
            DrawLine(tx,ry+rh,tx+SW-160,ry+rh,{C_BORDER.r,C_BORDER.g,C_BORDER.b,70});
        }
        char buf[40];sprintf(buf,"Total: %d torres activas",n);
        DrawText(buf,tx,ty+rh+n*rh+12,13,C_GREEN);
        DrawText("Diagrama Lista Secuencial:",tx,ty+rh+n*rh+36,12,C_MUTED);
        for(int i=0;i<min(n,6);i++){
            int bx=tx+i*130,by=ty+rh+n*rh+54;
            DrawRectangle(bx,by,118,40,C_PANEL2);DrawRectangleLines(bx,by,118,40,C_BORDER);
            char lb[20];sprintf(lb,"[%d] %s",i,lT.get(i).nombre.substr(0,8).c_str());
            DrawText(lb,bx+6,by+10,11,C_TEXT);
            if(i<min(n,6)-1)DrawText("->",bx+120,by+14,13,C_GREEN);
        }
        return SCR_SHOW_TORRES;
    }

    // ============================================================
    //  ELIMINAR TORRE
    // ============================================================
    Screen drawDelTorre(float dt,Estado& estado,ListaSecuencialTorres& lT,GameScreen& gs){
        if(fbTimer>0)fbTimer-=dt;
        header("Eliminar Torre","Lista Secuencial — Eliminar por ID",C_RED);
        if(btnVolver2())return SCR_MENU;
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            Vector2 mp=GetMousePosition();
            if(mp.x>SW/2-200&&mp.x<SW/2+200&&mp.y>126&&mp.y<160) campoActivo=0;
        }
        fDelId.activo=(campoActivo==0);fDelId.update();
        int cx=SW/2-200,cy=108;
        drawCampo(fDelId,cx,cy,400,"ID de la torre a eliminar (clic aqui para escribir)",campoActivo==0);cy+=60;
        int n=lT.getTamanio();
        DrawText("Torres disponibles (clic en fila para autocompletar ID):",cx,cy,13,C_MUTED);cy+=22;
        for(int i=0;i<n;i++){
            Torre&t=lT.get(i);
            int ry=cy+i*28;
            Vector2 mp=GetMousePosition();
            bool hover=mp.x>cx&&mp.x<cx+600&&mp.y>ry&&mp.y<ry+24;
            if(hover)DrawRectangle(cx,ry,600,24,C_PANEL2);
            char buf[80];sprintf(buf,"[ID:%d]  %-12s  Tipo:%-10s  Pos:%d  Danio:%d  Costo:$%d",
                t.id,t.nombre.c_str(),t.tipo.c_str(),t.posicion,t.danio,t.costo);
            DrawText(buf,cx+4,ry+4,12,hover?C_YELLOW:C_TEXT);
            if(hover&&IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                char bid[8];sprintf(bid,"%d",t.id);strcpy(fDelId.buf,bid);fDelId.len=strlen(bid);
                campoActivo=0;
            }
        }
        if(n==0)DrawText("(Sin torres registradas)",cx,cy,12,C_MUTED);
        cy+=max(n*28,28)+12;
        if(boton(cx,cy,400,44,"ELIMINAR TORRE",C_RED)||IsKeyPressed(KEY_ENTER)){
            int id=fDelId.toInt();
            if(id>0&&lT.eliminarPorId(id)){gs.remTorreVisual(id);fbMsg="Torre ID "+to_string(id)+" eliminada!";}
            else fbMsg="Error: ID "+to_string(id)+" no encontrado";
            fbTimer=3;fDelId.clear();campoActivo=-1;
        }
        feedback(fbMsg,fbTimer,cx,cy+54);
        return SCR_DEL_TORRE;
    }

    // ============================================================
    //  REGISTRAR OLEADA
    // ============================================================
    Screen drawRegOleada(float dt,ListaCircularOleadas& lO){
        if(fbTimer>0)fbTimer-=dt;
        header("Registrar Oleada","Lista Circular — Insertar al final del circulo",C_PURPLE);
        if(btnVolver2())return SCR_MENU;

        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            Vector2 mp=GetMousePosition();
            int cx=SW/2-300,cy=110;
            struct{int x,y,w,h;int idx;}areas[]={
                {cx,cy,120,34,0},{cx+140,cy,120,34,1},
                {cx,cy+64,260,34,2},{cx,cy+128,130,34,3},{cx+148,cy+128,130,34,4}
            };
            bool clicked=false;
            for(auto&a:areas) if(mp.x>a.x&&mp.x<a.x+a.w&&mp.y>a.y&&mp.y<a.y+a.h){campoActivo=a.idx;clicked=true;break;}
            if(!clicked) campoActivo=-1;
        }
        if(IsKeyPressed(KEY_TAB)) campoActivo=(campoActivo+1)%5;
        fOId.activo=(campoActivo==0);fOCant.activo=(campoActivo==1);fOTipo.activo=(campoActivo==2);
        fOVida.activo=(campoActivo==3);fOVel.activo=(campoActivo==4);
        fOId.update();fOCant.update();fOTipo.update();fOVida.update();fOVel.update();
        if(strlen(fOId.buf)==0){char b[8];sprintf(b,"%d",waveIdCnt);strcpy(fOId.buf,b);fOId.len=strlen(b);}

        int cx=SW/2-300,cy=110;

        // Rangos sugeridos como guia
        DrawRectangle(cx+300,cy-10,360,210,C_PANEL2);
        DrawRectangleLines(cx+300,cy-10,360,210,C_BORDER);
        DrawText("Rangos recomendados:",cx+312,cy+2,12,C_MUTED);
        DrawText("Enemigos:  min 1  |  max 20",cx+312,cy+22,11,C_TEXT);
        DrawText("Vida:      min 10 |  max 500",cx+312,cy+40,11,C_TEXT);
        DrawText("Velocidad: min 1  |  max 4",cx+312,cy+58,11,C_TEXT);
        DrawLine(cx+312,cy+76,cx+648,cy+76,C_BORDER);
        DrawText("Tipos sugeridos:",cx+312,cy+84,11,C_MUTED);
        struct{const char*nom;Color col;}tips[]={{"Basico",C_GREEN},{"Rapido",C_YELLOW},{"Tanque",C_BLUE},{"Jefe",C_RED}};
        for(int i=0;i<4;i++){
            DrawCircle(cx+322,cy+106+i*22,5,tips[i].col);
            DrawText(tips[i].nom,cx+334,cy+98+i*22,12,tips[i].col);
        }

        drawCampo(fOId,cx,cy,120,"ID Oleada",campoActivo==0);
        drawCampo(fOCant,cx+140,cy,120,"Cant. Enemigos (1-20)",campoActivo==1);cy+=64;
        drawCampo(fOTipo,cx,cy,260,"Tipo Enemigo (clic aqui)",campoActivo==2);cy+=64;
        drawCampo(fOVida,cx,cy,130,"Vida Base (10-500)",campoActivo==3);
        drawCampo(fOVel,cx+148,cy,130,"Velocidad (1-4)",campoActivo==4);cy+=58;
        DrawText("Clic en campo para escribir | TAB = siguiente campo",cx,cy-12,10,C_MUTED);

        if(boton(cx,cy,260,44,"REGISTRAR OLEADA",C_PURPLE)||IsKeyPressed(KEY_ENTER)){
            int cant=fOCant.toInt(), vida=fOVida.toInt(), vel=fOVel.toInt();
            if(strlen(fOTipo.buf)>0&&cant>0){
                // Validar rangos
                cant=max(1,min(cant,20));
                vida=max(10,min(vida,500));
                vel=max(1,min(vel,4));
                Oleada o;o.idOleada=fOId.toInt();o.cantidadEnemigos=cant;
                o.tipoEnemigo=fOTipo.buf;o.vidaBase=vida;o.velocidadBase=vel;
                lO.registrar(o);waveIdCnt++;
                fbMsg="Oleada registrada en lista circular!";fbTimer=3;
                fOId.clear();fOCant.clear();fOTipo.clear();fOVida.clear();fOVel.clear();campoActivo=-1;
            }else{fbMsg="Completa tipo y cantidad de enemigos!";fbTimer=3;}
        }
        feedback(fbMsg,fbTimer,cx,cy+54);

        // Mostrar lista circular con datos reales
        int n=lO.getTamanio();
        if(n>0){
            int dy=cy+80;
            DrawText("Lista Circular — nodos registrados:",cx,dy,12,C_MUTED);dy+=20;
            for(int i=0;i<min(n,5);i++){
                Oleada*ol=lO.getPorIndice(i);
                int bx=cx+i*150;
                DrawRectangle(bx,dy,138,56,C_PANEL2);DrawRectangleLines(bx,dy,138,56,i==0?C_PURPLE:C_BORDER);
                DrawText(TextFormat("ID:%d %s",ol->idOleada,ol->tipoEnemigo.substr(0,6).c_str()),bx+6,dy+6,11,C_TEXT);
                DrawText(TextFormat("Enem:%d V:%d",ol->cantidadEnemigos,ol->vidaBase),bx+6,dy+24,10,C_MUTED);
                DrawText(TextFormat("Vel:%d",ol->velocidadBase),bx+6,dy+40,10,C_MUTED);
                if(i<min(n,5)-1)DrawText("->",bx+130,dy+20,13,C_PURPLE);
                else if(n>1)DrawText("->[1]",bx+60,dy+60,10,C_PURPLE);
            }
        }
        return SCR_REG_OLEADA;
    }

    // ============================================================
    //  VER OLEADAS
    // ============================================================
    Screen drawShowOleadas(int oleadaIdx,ListaCircularOleadas& lO){
        header("Oleadas Registradas","Lista Circular — ultimo->siguiente apunta al primero",C_BLUE);
        if(btnVolver2())return SCR_MENU;
        int n=lO.getTamanio();
        if(n==0){subtitulo("No hay oleadas. Usa la opcion 5 para registrar.",370);return SCR_SHOW_OLEADAS;}
        int tx=80,ty=98,rh=40;
        int widths[]={50,140,100,100,100,100};
        const char*heads[]={"ID","Tipo","Enemigos","Vida Base","Velocidad","Recompensa"};
        DrawRectangle(tx,ty,SW-160,rh,C_PANEL2);int cx=tx+10;
        for(int i=0;i<6;i++){DrawText(heads[i],cx,ty+12,12,C_MUTED);cx+=widths[i];}
        DrawLine(tx,ty+rh,tx+SW-160,ty+rh,C_BORDER);
        // Leer datos REALES de la lista circular
        int oleadaActual = 0;
        Oleada* oActual = lO.getActual();
        if (oActual) oleadaActual = oActual->idOleada;

        for(int i=0;i<min(n,10);i++){
            Oleada*ol=lO.getPorIndice(i);
            if(!ol)continue;
            int ry=ty+rh+i*rh;bool esAct=(ol->idOleada==oleadaActual);
            DrawRectangle(tx,ry,SW-160,rh,i%2==0?C_PANEL:C_PANEL2);
            if(esAct)DrawRectangle(tx,ry,4,rh,C_ORANGE);
            cx=tx+10;char vs[6][32];
            sprintf(vs[0],"%d",ol->idOleada);
            sprintf(vs[1],"%s",ol->tipoEnemigo.substr(0,12).c_str());
            sprintf(vs[2],"%d",ol->cantidadEnemigos);
            sprintf(vs[3],"%d",ol->vidaBase);
            sprintf(vs[4],"%d",ol->velocidadBase);
            sprintf(vs[5],"$%d",10+ol->idOleada*5);
            for(int j=0;j<6;j++){DrawText(vs[j],cx,ry+12,12,esAct&&j==0?C_ORANGE:C_TEXT);cx+=widths[j];}
            if(esAct)DrawText("<-- ACTUAL",tx+SW-300,ry+12,12,C_ORANGE);
            DrawLine(tx,ry+rh,tx+SW-160,ry+rh,{C_BORDER.r,C_BORDER.g,C_BORDER.b,60});
        }
        int dy=ty+rh+min(n,10)*rh+16;
        DrawText("Diagrama Lista Circular (ultimo->siguiente = primero):",tx,dy,12,C_MUTED);dy+=20;
        int olaActual = 0;
        Oleada* olaActualPtr = lO.getActual();
        if (olaActualPtr) olaActual = olaActualPtr->idOleada;

        for(int i=0;i<min(n,6);i++){
            Oleada*ol=lO.getPorIndice(i);
            if(!ol)continue;
            int bx=tx+i*158;bool esAct=(ol->idOleada==olaActual);
            DrawRectangle(bx,dy,146,60,esAct?C_PANEL2:C_PANEL);
            DrawRectangleLines(bx,dy,146,60,esAct?C_ORANGE:C_BORDER);
            DrawText(TextFormat("ID:%d %s",ol->idOleada,ol->tipoEnemigo.substr(0,6).c_str()),bx+6,dy+6,11,esAct?C_ORANGE:C_TEXT);
            DrawText(TextFormat("E:%d V:%d Vel:%d",ol->cantidadEnemigos,ol->vidaBase,ol->velocidadBase),bx+6,dy+24,10,C_MUTED);
            DrawText("->siguiente",bx+6,dy+42,9,C_MUTED);
            if(i==min(n,6)-1){DrawText("->",bx+148,dy+24,14,C_PURPLE);DrawText("[1]",bx+164,dy+24,12,C_PURPLE);}
            else DrawText("->",bx+148,dy+24,14,C_BLUE);
        }
        return SCR_SHOW_OLEADAS;
    }

    // ============================================================
    //  ENEMIGOS ACTIVOS
    // ============================================================
    Screen drawShowEnemigos(ListaDobleEnemigos& lE){
        header("Enemigos Activos","Lista Doblemente Enlazada — primero <-> ... <-> ultimo",C_RED);
        if(btnVolver2())return SCR_MENU;
        int n=lE.getTamanio();
        if(n==0){subtitulo("No hay enemigos activos. Inicia una oleada desde el juego.",370);return SCR_SHOW_ENEMIGOS;}
        int tx=80,ty=98,rh=38;
        int widths[]={60,130,80,100,100,110};
        const char*heads[]={"ID","Tipo","Vida","Velocidad","Posicion","Recompensa"};
        DrawRectangle(tx,ty,SW-160,rh,C_PANEL2);int cx=tx+8;
        for(int i=0;i<6;i++){DrawText(heads[i],cx,ty+10,12,C_MUTED);cx+=widths[i];}
        DrawLine(tx,ty+rh,tx+SW-160,ty+rh,C_BORDER);
        NodoEnemigo*ne=lE.getPrimero();int i=0;
        while(ne&&i<12){
            int ry=ty+rh+i*rh;
            DrawRectangle(tx,ry,SW-160,rh,i%2==0?C_PANEL:C_PANEL2);
            DrawText(ne->anterior?"<-":"NULL",tx-36,ry+10,10,C_BLUE);
            DrawText(ne->siguiente?"->":"NULL",tx+SW-155,ry+10,10,C_BLUE);
            cx=tx+8;char vs[6][32];
            sprintf(vs[0],"%d",ne->dato.id);sprintf(vs[1],"%s",ne->dato.tipo.c_str());
            sprintf(vs[2],"%d",ne->dato.vida);sprintf(vs[3],"%d",ne->dato.velocidad);
            sprintf(vs[4],"%d",ne->dato.posicion);sprintf(vs[5],"$%d",ne->dato.recompensa);
            for(int j=0;j<6;j++){DrawText(vs[j],cx,ry+10,12,C_TEXT);cx+=widths[j];}
            DrawLine(tx,ry+rh,tx+SW-160,ry+rh,{C_BORDER.r,C_BORDER.g,C_BORDER.b,60});
            ne=ne->siguiente;i++;
        }
        char buf[40];sprintf(buf,"Total: %d enemigos activos",n);
        DrawText(buf,tx,ty+rh+i*rh+10,13,C_RED);
        DrawText("NULL <- [primero] <-> [nodo] <-> ... <-> [ultimo] -> NULL",tx,ty+rh+i*rh+32,12,C_MUTED);
        return SCR_SHOW_ENEMIGOS;
    }

    // ============================================================
    //  ESTADO GENERAL
    // ============================================================
    Screen drawEstado(const Estado& estado,int oleadaIdx,
                      ListaSecuencialTorres& lT,ListaDobleEnemigos& lE,ListaCircularOleadas& lO){
        header("Estado General del Juego","Resumen completo de estructuras y estado",C_GREEN);
        if(btnVolver2())return SCR_MENU;
        char buf[120];int lx=80,rx=SW/2+40,y=100;
        DrawText("JUGADOR",lx,y,13,C_MUTED);y+=24;
        DrawText("Vidas:",lx,y,13,C_TEXT);sprintf(buf,"%d/3",estado.vidas);DrawText(buf,lx+180,y,13,C_RED);y+=22;
        DrawText("Dinero:",lx,y,13,C_TEXT);sprintf(buf,"$%d",estado.dinero);DrawText(buf,lx+180,y,13,C_YELLOW);y+=22;
        DrawText("Puntuacion:",lx,y,13,C_TEXT);sprintf(buf,"%d",estado.puntuacion);DrawText(buf,lx+180,y,13,C_BLUE);y+=22;
        int oleadaActual = 0;
        Oleada* oActual = lO.getActual();
        if (oActual) oleadaActual = oActual->idOleada;
        DrawText("Turno:",lx,y,13,C_TEXT);sprintf(buf,"%d",estado.turno);DrawText(buf,lx+180,y,13,C_GREEN);y+=22;
        DrawText("Oleada:",lx,y,13,C_TEXT);sprintf(buf,"%d/4",oleadaActual);DrawText(buf,lx+180,y,13,C_ORANGE);y+=22;

        DrawText("Estado:",lx,y,13,C_TEXT);
        DrawText(estado.gameOver?"GAME OVER":estado.victoria?"VICTORIA":"En progreso",lx+180,y,13,
            estado.gameOver?C_RED:estado.victoria?C_GREEN:C_CYAN);
        y=100;DrawText("ESTRUCTURAS",rx,y,13,C_MUTED);y+=24;        sprintf(buf,"%d torres",lT.getTamanio());DrawText("Lista Secuencial:",rx,y,13,C_TEXT);DrawText(buf,rx+200,y,13,C_GREEN);y+=22;
        sprintf(buf,"%d enemigos",lE.getTamanio());DrawText("Lista Doble Enlazada:",rx,y,13,C_TEXT);DrawText(buf,rx+200,y,13,C_BLUE);y+=22;
        sprintf(buf,"%d oleadas",lO.getTamanio());DrawText("Lista Circular:",rx,y,13,C_TEXT);DrawText(buf,rx+200,y,13,C_PURPLE);
        y=252;DrawLine(lx,y,SW-lx,y,C_BORDER);y+=14;
        DrawText("LISTA SECUENCIAL — Torres:",lx,y,12,C_MUTED);y+=20;
        for(int i=0;i<min(lT.getTamanio(),4);i++){
            Torre&t=lT.get(i);
            sprintf(buf,"[ID:%d] %-12s Tipo:%-10s Pos:%2d Danio:%3d Rango:%d Costo:$%d",
                t.id,t.nombre.c_str(),t.tipo.c_str(),t.posicion,t.danio,t.rango,t.costo);
            DrawText(buf,lx,y+i*20,12,C_TEXT);
        }
        if(lT.getTamanio()==0)DrawText("(vacia)",lx,y,12,C_MUTED);
        y+=max(lT.getTamanio()*20,20)+14;
        DrawLine(lx,y,SW-lx,y,C_BORDER);y+=14;
        DrawText("LISTA DOBLEMENTE ENLAZADA — Enemigos:",lx,y,12,C_MUTED);y+=20;
        NodoEnemigo*ne=lE.getPrimero();int ci=0;
        while(ne&&ci<4){
            sprintf(buf,"[ID:%d] Tipo:%-8s Vida:%3d Vel:%d Pos:%2d Rew:$%d",
                ne->dato.id,ne->dato.tipo.c_str(),ne->dato.vida,ne->dato.velocidad,ne->dato.posicion,ne->dato.recompensa);
            DrawText(buf,lx,y+ci*20,12,C_TEXT);ne=ne->siguiente;ci++;
        }
        if(lE.getTamanio()==0)DrawText("(vacia)",lx,y,12,C_MUTED);
        y+=max(ci*20,20)+14;
        DrawLine(lx,y,SW-lx,y,C_BORDER);y+=14;
        DrawText("REGLAS DEL JUEGO (PDF):",lx,y,12,C_MUTED);y+=20;
        const char*r[]={"1. Ruta: posiciones 0 a 20","2. Enemigos aparecen en posicion 0",
            "3. Cada turno enemigos avanzan segun velocidad","4. Torres atacan en rango automaticamente",
            "5. Enemigo con vida=0 se elimina de la lista","6. Enemigo al final: -1 vida al jugador",
            "7. Fin: sin vidas o todas las oleadas completadas"};
        for(int i=0;i<7;i++)DrawText(r[i],lx,y+i*18,11,C_TEXT);
        return SCR_ESTADO;
    }

    // ============================================================
    //  INICIAR OLEADA — seleccionar cuál oleada iniciar
    // ============================================================
    Screen drawIniciarOleada(float dt, GameScreen& gs, ListaCircularOleadas& lO){
        header("Iniciar Oleada","Selecciona la oleada con la que quieres empezar",C_ORANGE);
        if(btnVolver2()) return SCR_MENU;

        int n=lO.getTamanio();
        if(n==0){
            subtitulo("No hay oleadas registradas. Usa la opcion 5 para registrar.",370);
            return SCR_INICIAR_OLEADA;
        }

        DrawText("Haz clic en la oleada que quieres iniciar:",SW/2-220,100,14,C_TEXT);

        // Datos de las oleadas
        Color defCols[]={C_GREEN,C_YELLOW,C_BLUE,C_RED,C_CYAN,C_PURPLE,C_ORANGE,C_GREEN};

        int cardW=240, cardH=130, gap=20;
        int perRow=min(n,4);
        int totalW = perRow*(cardW+gap)-gap;
        int startX = SW/2 - totalW/2;
        int startY = 140;

        Screen next=SCR_INICIAR_OLEADA;
        Vector2 mp=GetMousePosition();

        for(int i=0;i<min(n,8);i++){
            int row=i/4, col2=i%4;
            int bx=startX+col2*(cardW+gap), by=startY+row*(cardH+gap);
            bool hover=mp.x>bx&&mp.x<bx+cardW&&mp.y>by&&mp.y<by+cardH;
            bool esActual=(i+1==gs.oleadaIdx);

            // Leer datos REALES
            Oleada*ol=lO.getPorIndice(i);
            if(!ol)continue;

            Color col=defCols[i%8];
            DrawRectangleRounded({(float)bx,(float)by,(float)cardW,(float)cardH},0.12f,8,
                hover?C_PANEL2:C_PANEL);
            DrawRectangleLinesEx({(float)bx,(float)by,(float)cardW,(float)cardH},
                hover?2:1, hover?col:C_BORDER);

            // Número grande
            char num[4]; sprintf(num,"%d",i+1);
            int nw=MeasureText(num,40);
            DrawText(num,bx+cardW/2-nw/2,by+10,40,col);

            // Info real de la oleada
            DrawText(ol->tipoEnemigo.c_str(),bx+cardW/2-MeasureText(ol->tipoEnemigo.c_str(),13)/2,by+58,13,C_TEXT);
            char info[50]; sprintf(info,"%d enem  Vida:%d  Vel:%d",ol->cantidadEnemigos,ol->vidaBase,ol->velocidadBase);
            DrawText(info,bx+cardW/2-MeasureText(info,10)/2,by+78,10,C_MUTED);
            char recp[20]; sprintf(recp,"Recomp: $%d",10+ol->idOleada*5);
            DrawText(recp,bx+cardW/2-MeasureText(recp,10)/2,by+96,10,C_MUTED);

            if(esActual)
                DrawText("EN CURSO",bx+cardW/2-MeasureText("EN CURSO",10)/2,by+114,10,C_ORANGE);

            if(hover&&IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                lO.reiniciar();
                for(int j=0;j<i;j++) lO.avanzar();
                Oleada*olS=lO.getActual();
                if(olS){
                    gs.oleadaActiva=true;
                    gs.spawneados=0;
                    gs.totalSpawn=olS->cantidadEnemigos;
                    gs.spawnTimer=0;
                    gs.oleadaIdx=olS->idOleada;
                    gs.msg="Oleada "+to_string(olS->idOleada)+" — "+olS->tipoEnemigo+" iniciada!";
                    gs.msgTimer=4;
                }
                next=SCR_JUEGO;
            }
        }

        // Segunda fila si hay más de 4
        if(n>4){
            int by2=startY+cardH+gap;
            for(int i=4;i<min(n,8);i++){
                int bx=startX+(i-4)*(cardW+gap);
                bool hover=mp.x>bx&&mp.x<bx+cardW&&mp.y>by2&&mp.y<by2+cardH;
                Color col=C_CYAN;
                DrawRectangleRounded({(float)bx,(float)by2,(float)cardW,(float)cardH},0.12f,8,hover?C_PANEL2:C_PANEL);
                DrawRectangleLinesEx({(float)bx,(float)by2,(float)cardW,(float)cardH},hover?2:1,hover?col:C_BORDER);
                char num[4]; sprintf(num,"%d",i+1); int nw=MeasureText(num,40);
                DrawText(num,bx+cardW/2-nw/2,by2+10,40,col);
                DrawText("Custom",bx+cardW/2-MeasureText("Custom",13)/2,by2+58,13,C_TEXT);
                if(hover&&IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                    lO.reiniciar();for(int j=0;j<i;j++)lO.avanzar();
                    Oleada*ol=lO.getActual();
                    if(ol){gs.oleadaActiva=true;gs.spawneados=0;gs.totalSpawn=ol->cantidadEnemigos;
                        gs.spawnTimer=0;gs.oleadaIdx=ol->idOleada;gs.msg="Oleada "+to_string(ol->idOleada)+" iniciada!";gs.msgTimer=4;}
                    next=SCR_JUEGO;
                }
            }
        }

        // Nota
        DrawText("Tip: desde el juego puedes presionar M para volver al menu",
            SW/2-MeasureText("Tip: desde el juego puedes presionar M para volver al menu",11)/2,
            SH-80,11,C_MUTED);

        return next;
    }

    // ============================================================
    //  AVANZAR TURNO — resumen del turno ejecutado
    // ============================================================
    struct ResumenTurno {
        int turnoNum=0;
        int enemigosMov=0, torresDispararon=0, enemigosEliminados=0;
        int enemigosMeta=0, danioTotal=0;
        string log[20]; int nLog=0;
        void addLog(const string& s){ if(nLog<20) log[nLog++]=s; }
    };
    vector<ResumenTurno> historialTurnos;
    bool turnoEjecutado=false;
    int indiceTurno=0; // 0 = último, 1 = penúltimo, etc.

    void ejecutarTurno(Estado& estado, ListaSecuencialTorres& lT,
                       ListaDobleEnemigos& lE, ListaCircularOleadas& lO, GameScreen& gs){
        ResumenTurno ultimoTurno;
        ultimoTurno.turnoNum = estado.turno + 1; // Próximo turno
        estado.turno++;
        char buf[80]; sprintf(buf,"=== TURNO %d ===",estado.turno);
        ultimoTurno.addLog(buf);

        int beforeEnemigos = lE.getTamanio();
        int beforeVidas = estado.vidas;

        // Ejecuta la lógica central de un tick usando la simulación del game.
        gs.update(1.0f/14.0f, estado, lT, lE, lO);

        // Registros simplificados que ahora siguen la misma ruta de movimiento real.
        int afterEnemigos = lE.getTamanio();
        ultimoTurno.enemigosMov = (beforeEnemigos > 0 ? 1 : 0);
        ultimoTurno.enemigosEliminados = max(0, beforeEnemigos - afterEnemigos);
        ultimoTurno.enemigosMeta = beforeVidas - estado.vidas;

        sprintf(buf,"Enemigos: %d -> %d",beforeEnemigos,afterEnemigos);
        ultimoTurno.addLog(buf);
        sprintf(buf,"Vidas: %d -> %d", beforeVidas, estado.vidas);
        ultimoTurno.addLog(buf);

        sprintf(buf,"--- Fin turno %d: %d enemigos eliminados, %d en meta ---",
            estado.turno,ultimoTurno.enemigosEliminados,ultimoTurno.enemigosMeta);
        ultimoTurno.addLog(buf);

        historialTurnos.push_back(ultimoTurno);
        turnoEjecutado=true;
    }

    Screen drawAvanzarTurno(float dt, Estado& estado, ListaSecuencialTorres& lT,
                             ListaDobleEnemigos& lE, ListaCircularOleadas& lO, GameScreen& gs){
        header("Skip Ronda","Registra la info de cada ronda y revisa con ANTERIOR/SIGUIENTE",C_YELLOW);
        if(btnVolver2()) return SCR_MENU;

        if(!turnoEjecutado){
            // Ejecutar el turno
            if(lE.getTamanio()==0&&gs.oleadaIdx==0){
                subtitulo("No hay enemigos activos. Inicia una oleada primero (opcion 7).",370);
                return SCR_AVANZAR_TURNO;
            }
            ejecutarTurno(estado,lT,lE,lO,gs);
        }

        // Mostrar resumen
        int lx=60, y=100;
        if(historialTurnos.empty()){
            DrawText("No hay turnos ejecutados aún.",lx,y,14,C_MUTED);
            return SCR_AVANZAR_TURNO;
        }
        ResumenTurno& rt = historialTurnos[historialTurnos.size() - 1 - indiceTurno];
        int mostrando = (int)historialTurnos.size() - indiceTurno; // 1 = turno inicial, n = turno más reciente
        DrawText(TextFormat("TURNO %d (Mostrando %d de %d)", rt.turnoNum, mostrando, (int)historialTurnos.size()), lx, y, 18, C_YELLOW); y += 36;

        // Stats del turno en cards
        int cw=200,ch=80,gap=16;
        struct Card{const char*label;int val;Color col;};
        Card cards[]={
            {"Enemigos movidos",rt.enemigosMov,C_BLUE},
            {"Torres dispararon",rt.torresDispararon,C_GREEN},
            {"Enemigos eliminados",rt.enemigosEliminados,C_RED},
            {"Llegaron a la meta",rt.enemigosMeta,C_ORANGE},
            {"Danio total",rt.danioTotal,C_PURPLE},
        };
        y += 40;
        for(int i=0;i<5;i++){
            int bx=lx+i*(cw+gap);
            DrawRectangle(bx,y,cw,ch,C_PANEL2);
            DrawRectangleLines(bx,y,cw,ch,cards[i].col);
            char val[8]; sprintf(val,"%d",cards[i].val);
            int vw=MeasureText(val,28); DrawText(val,bx+cw/2-vw/2,y+12,28,cards[i].col);
            int lw=MeasureText(cards[i].label,10); DrawText(cards[i].label,bx+cw/2-lw/2,y+50,10,C_MUTED);
        }
        y+=ch+20;

        // Log detallado
        DrawText("LOG DEL TURNO:",lx,y,12,C_MUTED); y+=18;
        DrawRectangle(lx,y,SW-lx*2,rt.nLog*20+16,C_PANEL);
        DrawRectangleLines(lx,y,SW-lx*2,rt.nLog*20+16,C_BORDER);
        for(int i=0;i<rt.nLog;i++){
            Color lc=C_TEXT;
            if(rt.log[i].find("ELIMINADO")!=string::npos) lc=C_RED;
            else if(rt.log[i].find("ataca")!=string::npos) lc=C_GREEN;
            else if(rt.log[i].find("BASE")!=string::npos) lc=C_ORANGE;
            else if(rt.log[i].find("===")!=string::npos) lc=C_YELLOW;
            DrawText(rt.log[i].c_str(),lx+10,y+8+i*20,11,lc);
        }
        y+=rt.nLog*20+24;

        // Estado actual
        DrawText(TextFormat("Vidas: %d  |  Dinero: $%d  |  Puntuacion: %d  |  Enemigos activos: %d",
            estado.vidas,estado.dinero,estado.puntuacion,lE.getTamanio()),lx,y,12,C_TEXT);

        if(estado.gameOver){
            DrawText("!! GAME OVER — El jugador ha perdido todas las vidas !!",
                lx,y+24,13,C_RED);
        }

        // Botones de navegacion de historial
        if(boton(SW-760,SH-60,200,38,"SIGUIENTE (historial)",C_GREEN) && indiceTurno > 0){
            indiceTurno--;
        }
        if(boton(SW-540,SH-60,200,38,"ANTERIOR (historial)",C_BLUE) && indiceTurno < (int)historialTurnos.size()-1){
            indiceTurno++;
        }
        // Boton salto oleada (fast-forward)
        if(boton(SW-320,SH-60,220,36,"SALTAR OLEADA",C_PURPLE)){
            gs.skipCurrentOleada(estado,lT,lE,lO);
            turnoEjecutado = true;
            indiceTurno = 0;
        }

        DrawText("Usa ANTERIOR/SIGUIENTE para historial; SALTAR OLEADA = termina inmediamente.",
            SW-640, SH-96, 10, C_MUTED);

        return SCR_AVANZAR_TURNO;
    }
};
