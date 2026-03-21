#pragma once
#include "raylib.h"
#include "Helpers.h"
#include "Estado.h"
#include "ListaTorres.h"
#include "ListaEnemigos.h"
#include "ListaOleadas.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
using namespace std;

// ============================================================
//  MAPA
// ============================================================
const int CELL=36, GCOLS=26, GROWS=18;
const int MAP_X=260, MAP_Y=0;
const int MAP_W=GCOLS*CELL, MAP_H=GROWS*CELL;

struct Pt { int x,y; };
const Pt WP[]={{0,8},{4,8},{4,3},{9,3},{9,8},{14,8},{14,3},{19,3},{19,8},{21,8},{21,3},{25,3},{25,8},{GCOLS,8}};
const int N_WP=14;

// ============================================================
//  TIPOS DE TORRE
// ============================================================
struct TipoDef { const char* nom; int costo,danio,rango; Color col; float rate; };
const TipoDef TIPOS[4]={
    {"Arquero", 50,20,3,C_GREEN, 1.2f},
    {"Canon",  100,55,2,C_BLUE,  0.6f},
    {"Hielo",   75,10,3,C_PURPLE,0.9f},
    {"Laser",  150,80,4,C_YELLOW,0.4f},
};
const int N_TIPOS=4;

// ============================================================
//  STRUCTS VISUALES
// ============================================================
struct TorreVis  { int id,col,row,tipo; float cd; };
struct EnemigoVis{ int id,hp,mhp,rew,wpI; float x,y,spd; string tipo; bool vivo; Color col; };
struct Proj      { float x,y,tx,ty,spd; Color col; int tid,dmg; bool done; };
struct Part      { float x,y,vx,vy,life,ml; Color col; };

// ============================================================
//  CLASE GameScreen
// ============================================================
class GameScreen {
public:
    TorreVis    torresV[100];  int nT=0;
    EnemigoVis  enemigosV[300];int nE=0;
    Proj        projs[600];    int nP=0;
    Part        parts[800];    int nPt=0;

    int    selTipo=0;
    bool   oleadaActiva=false;
    int    oleadaIdx=0,spawneados=0,totalSpawn=0;
    float  spawnTimer=0;
    string msg="Clic=torre | ESPACIO=oleada | T=turno | M=menu";
    float  msgTimer=5;

    // ---- helpers camino ----
    bool enCamino(int c,int r) const {
        for(int i=0;i<N_WP-1;i++){
            Pt a=WP[i],b=WP[i+1];
            if(a.x==b.x){int mn=min(a.y,b.y),mx=max(a.y,b.y);if(c==a.x&&r>=mn&&r<=mx)return true;}
            else         {int mn=min(a.x,b.x),mx=max(a.x,b.x);if(r==a.y&&c>=mn&&c<=mx)return true;}
        }
        return false;
    }
    Vector2 wpPx(int i) const {
        return {MAP_X+WP[i].x*CELL+CELL/2.0f, MAP_Y+WP[i].y*CELL+CELL/2.0f};
    }

    // ---- añadir torre visual ----
    void addTorreVisual(int id,int col,int row,int tipo){
        if(nT<100){ TorreVis tv; tv.id=id;tv.col=col;tv.row=row;tv.tipo=tipo;tv.cd=0; torresV[nT++]=tv; }
    }
    void remTorreVisual(int id){
        for(int i=0;i<nT;i++) if(torresV[i].id==id){ for(int j=i;j<nT-1;j++) torresV[j]=torresV[j+1]; nT--; return; }
    }

    // ---- update ----
    void update(float dt, Estado& estado, ListaSecuencialTorres& lT, ListaDobleEnemigos& lE, ListaCircularOleadas& lO){
        if(estado.gameOver||estado.victoria) return;
        if(msgTimer>0) msgTimer-=dt;

        // Spawn
        if(oleadaActiva){
            spawnTimer-=dt;
            if(spawnTimer<=0&&spawneados<totalSpawn){
                Oleada*ol=lO.getActual();
                if(ol){
                    EnemigoVis ev;
                    ev.id=estado.idEnemigo; ev.wpI=0;
                    Vector2 p=wpPx(0); ev.x=p.x; ev.y=p.y;
                    ev.hp=ol->vidaBase; ev.mhp=ol->vidaBase;
                    ev.spd=ol->velocidadBase*52.0f;
                    ev.rew=10+ol->idOleada*5; ev.tipo=ol->tipoEnemigo; ev.vivo=true;
                    Color cols[]={C_RED,C_ORANGE,{200,50,50,255},C_PURPLE};
                    ev.col=cols[ol->idOleada%4];
                    if(nE<300) enemigosV[nE++]=ev;
                    Enemigo e; e.id=estado.idEnemigo++; e.tipo=ol->tipoEnemigo;
                    e.vida=ol->vidaBase; e.velocidad=ol->velocidadBase; e.posicion=0; e.recompensa=ev.rew;
                    lE.insertarAlFinal(e);
                    spawneados++; spawnTimer=0.9f;
                }
            }
        }

        // Mover enemigos
        for(int i=0;i<nE;i++){
            if(!enemigosV[i].vivo) continue;
            EnemigoVis& ev=enemigosV[i];
            int nx=ev.wpI+1;
            if(nx>=N_WP){
                ev.vivo=false; estado.vidas--;
                lE.eliminarPorId(ev.id);
                msg="Enemigo llego a la BASE! -1 vida"; msgTimer=3;
                if(estado.vidas<=0) estado.gameOver=true;
                continue;
            }
            Vector2 t=wpPx(nx);
            float dx=t.x-ev.x,dy=t.y-ev.y,dist=sqrtf(dx*dx+dy*dy),step=ev.spd*dt;
            if(dist<=step){ev.x=t.x;ev.y=t.y;ev.wpI++;}
            else{ev.x+=dx/dist*step;ev.y+=dy/dist*step;}
            NodoEnemigo*ne=lE.buscarPorId(ev.id);
            if(ne) ne->dato.posicion=(int)((ev.x-MAP_X)/CELL);
        }

        // Torres atacan
        for(int i=0;i<nT;i++){
            torresV[i].cd-=dt;
            if(torresV[i].cd>0) continue;
            float cx=MAP_X+torresV[i].col*CELL+CELL/2.0f;
            float cy=MAP_Y+torresV[i].row*CELL+CELL/2.0f;
            float rng=TIPOS[torresV[i].tipo].rango*CELL*1.3f;
            for(int e=0;e<nE;e++){
                if(!enemigosV[e].vivo) continue;
                float dx=enemigosV[e].x-cx, dy=enemigosV[e].y-cy;
                if(sqrtf(dx*dx+dy*dy)<=rng){
                    if(nP<599){
                        Proj pr; pr.x=cx;pr.y=cy;pr.tx=enemigosV[e].x;pr.ty=enemigosV[e].y;
                        pr.spd=400;pr.col=TIPOS[torresV[i].tipo].col;pr.tid=enemigosV[e].id;
                        pr.dmg=TIPOS[torresV[i].tipo].danio;pr.done=false;
                        projs[nP++]=pr;
                    }
                    torresV[i].cd=1.0f/TIPOS[torresV[i].tipo].rate;
                    break;
                }
            }
        }

        // Proyectiles
        for(int i=0;i<nP;i++){
            if(projs[i].done) continue;
            Proj&pr=projs[i];
            for(int e=0;e<nE;e++) if(enemigosV[e].id==pr.tid&&enemigosV[e].vivo){pr.tx=enemigosV[e].x;pr.ty=enemigosV[e].y;break;}
            float dx=pr.tx-pr.x,dy=pr.ty-pr.y,dist=sqrtf(dx*dx+dy*dy),step=pr.spd*dt;
            if(dist<=step||dist<4){
                pr.done=true;
                for(int e=0;e<nE;e++) if(enemigosV[e].id==pr.tid&&enemigosV[e].vivo){
                    enemigosV[e].hp-=pr.dmg;
                    NodoEnemigo*ne=lE.buscarPorId(pr.tid);
                    if(ne) ne->dato.vida=max(0,enemigosV[e].hp);
                    for(int k=0;k<6;k++){
                        float a=((float)rand()/RAND_MAX)*6.28f,s=50+rand()%80;
                        if(nPt<799) parts[nPt++]={enemigosV[e].x,enemigosV[e].y,cosf(a)*s,sinf(a)*s-40,0.5f,0.5f,pr.col};
                    }
                    if(enemigosV[e].hp<=0){
                        enemigosV[e].vivo=false;
                        estado.dinero+=enemigosV[e].rew;
                        estado.puntuacion+=enemigosV[e].rew*10;
                        lE.eliminarPorId(pr.tid);
                    }
                    break;
                }
            } else { pr.x+=dx/dist*step; pr.y+=dy/dist*step; }
        }
        int np2=0; for(int i=0;i<nP;i++) if(!projs[i].done) projs[np2++]=projs[i]; nP=np2;

        // Partículas
        for(int i=0;i<nPt;i++){parts[i].x+=parts[i].vx*dt;parts[i].y+=parts[i].vy*dt;parts[i].vy+=120*dt;parts[i].life-=dt;}
        int np3=0; for(int i=0;i<nPt;i++) if(parts[i].life>0) parts[np3++]=parts[i]; nPt=np3;

        // Fin oleada
        if(oleadaActiva&&spawneados>=totalSpawn){
            bool todos=true;
            for(int i=0;i<nE;i++) if(enemigosV[i].vivo){todos=false;break;}
            if(todos){
                oleadaActiva=false; estado.dinero+=75;
                if(oleadaIdx>=4) estado.victoria=true;
                else{ msg="Oleada completada! +$75 — ESPACIO para continuar"; msgTimer=4; }
            }
        }
    }

    // ---- input ----
    Screen handleInput(float dt, Estado& estado, ListaSecuencialTorres& lT, ListaDobleEnemigos& lE, ListaCircularOleadas& lO){
        if(IsKeyPressed(KEY_ONE))   selTipo=0;
        if(IsKeyPressed(KEY_TWO))   selTipo=1;
        if(IsKeyPressed(KEY_THREE)) selTipo=2;
        if(IsKeyPressed(KEY_FOUR))  selTipo=3;
        if(IsKeyPressed(KEY_M))     return SCR_MENU;

        if(IsKeyPressed(KEY_T)&&!estado.gameOver&&!estado.victoria){
            estado.turno++;
            char b[40]; sprintf(b,"Turno %d ejecutado",estado.turno);
            msg=b; msgTimer=2;
        }

        if(IsKeyPressed(KEY_SPACE)&&!oleadaActiva&&!estado.gameOver&&!estado.victoria){
            if(!lO.estaVacia()){
                if(oleadaIdx > 0) lO.avanzar(); // la primera vez usa la actual; luego avanza
                Oleada*ol=lO.getActual();
                if(ol){
                    oleadaActiva=true; spawneados=0;
                    totalSpawn=ol->cantidadEnemigos; spawnTimer=0;
                    oleadaIdx = ol->idOleada; // sincroniza índice con la oleada actual
                    char b[80]; sprintf(b,"Oleada %d — %s (%d enemigos)",ol->idOleada,ol->tipoEnemigo.c_str(),ol->cantidadEnemigos);
                    msg=b; msgTimer=4;
                }
            } else { msg="No hay oleadas! Registra una (opcion 4)"; msgTimer=3; }
        }

        // Clic en mapa
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)&&!estado.gameOver&&!estado.victoria){
            Vector2 mp=GetMousePosition();
            if(mp.x>MAP_X&&mp.x<MAP_X+MAP_W&&mp.y>MAP_Y&&mp.y<MAP_Y+MAP_H){
                int col=(int)((mp.x-MAP_X)/CELL), row=(int)((mp.y-MAP_Y)/CELL);
                if(col>=0&&col<GCOLS&&row>=0&&row<GROWS&&!enCamino(col,row)){
                    bool ok=true;
                    for(int i=0;i<nT;i++) if(torresV[i].col==col&&torresV[i].row==row){ok=false;break;}
                    if(ok){
                        int costo=TIPOS[selTipo].costo;
                        if(estado.dinero>=costo){
                            estado.dinero-=costo;
                            addTorreVisual(estado.idTorre,col,row,selTipo);
                            Torre t; t.id=estado.idTorre++; t.nombre=TIPOS[selTipo].nom; t.tipo=TIPOS[selTipo].nom;
                            t.posicion=col; t.danio=TIPOS[selTipo].danio; t.rango=TIPOS[selTipo].rango; t.costo=costo;
                            lT.insertar(t);
                            char b[60]; sprintf(b,"Torre %s colocada!",TIPOS[selTipo].nom);
                            msg=b; msgTimer=2;
                        } else { msg="Dinero insuficiente!"; msgTimer=2; }
                    }
                }
            }
        }
        return SCR_JUEGO;
    }

    // ---- draw ----
    void draw(const Estado& estado, const ListaSecuencialTorres& lT, const ListaDobleEnemigos& lE, const ListaCircularOleadas& lO){
        // Panel izquierdo
        DrawRectangle(0,0,258,SH,C_PANEL);
        DrawLine(258,0,258,SH,C_BORDER);

        char buf[80];
        // Título
        DrawRectangle(0,0,258,56,C_PANEL2);
        DrawLine(0,56,258,56,C_BORDER);
        DrawText("TOWER DEFENSE",10,10,16,C_GREEN);
        DrawText("Estructura de Datos C++",10,32,10,C_MUTED);

        // Stats
        int y=66;
        DrawText("ESTADO",10,y,11,C_MUTED); y+=18;
        DrawText("Vidas",10,y,12,C_TEXT);
        for(int i=0;i<3;i++) DrawRectangle(90+i*22,y,16,14,i<estado.vidas?C_RED:C_PANEL2);
        y+=20;
        sprintf(buf,"$%d",estado.dinero);  DrawText("Dinero",10,y,12,C_TEXT); DrawText(buf,90,y,12,C_YELLOW);  y+=18;
        sprintf(buf,"%d",estado.puntuacion);DrawText("Puntos",10,y,12,C_TEXT); DrawText(buf,90,y,12,C_BLUE);    y+=18;
        sprintf(buf,"%d",estado.turno);    DrawText("Turno", 10,y,12,C_TEXT); DrawText(buf,90,y,12,C_GREEN);   y+=18;
        sprintf(buf,"%d/4",oleadaIdx);     DrawText("Oleada",10,y,12,C_TEXT); DrawText(buf,90,y,12,C_ORANGE);  y+=20;

        DrawLine(0,y,258,y,C_BORDER); y+=8;

        // Selector torres
        DrawText("TORRES  [1][2][3][4]",10,y,11,C_MUTED); y+=16;
        for(int i=0;i<N_TIPOS;i++){
            bool sel=selTipo==i;
            DrawRectangle(6,y+i*30,246,26,sel?C_PANEL2:C_PANEL);
            if(sel) DrawRectangleLines(6,y+i*30,246,26,TIPOS[i].col);
            DrawCircle(20,y+i*30+13,6,TIPOS[i].col);
            sprintf(buf,"[%d] %-8s $%d D:%d R:%d",i+1,TIPOS[i].nom,TIPOS[i].costo,TIPOS[i].danio,TIPOS[i].rango);
            DrawText(buf,32,y+i*30+7,11,sel?C_TEXT:C_MUTED);
        }
        y+=N_TIPOS*30+8;

        DrawLine(0,y,258,y,C_BORDER); y+=8;

        // Botón oleada
        bool puedeOleada=!oleadaActiva&&!estado.gameOver&&!estado.victoria;
        DrawRectangle(6,y,246,28,puedeOleada?C_PANEL2:C_PANEL);
        DrawRectangleLines(6,y,246,28,puedeOleada?C_GREEN:C_BORDER);
        DrawText(puedeOleada?"[SPACE] Iniciar Oleada":"Oleada en progreso...",14,y+8,11,puedeOleada?C_GREEN:C_MUTED);
        y+=36;
        DrawText("[T] Turno  [M] Menu  [ESC] Salir",10,y,10,C_MUTED); y+=22;

        DrawLine(0,y,258,y,C_BORDER); y+=8;

        // Estructuras en vivo
        DrawText("ESTRUCTURAS",10,y,11,C_MUTED); y+=16;
        DrawCircle(16,y+4,4,C_GREEN);  sprintf(buf,"%d torres (Lista Sec.)",lT.getTamanio());  DrawText(buf,24,y,11,C_TEXT); y+=18;
        DrawCircle(16,y+4,4,C_BLUE);   sprintf(buf,"%d enemigos (Doble Enl.)",lE.getTamanio());DrawText(buf,24,y,11,C_TEXT); y+=18;
        DrawCircle(16,y+4,4,C_PURPLE); sprintf(buf,"%d oleadas (Circ.)",lO.getTamanio());      DrawText(buf,24,y,11,C_TEXT); y+=20;

        DrawLine(0,y,258,y,C_BORDER); y+=8;

        // Últimos enemigos activos
        DrawText("ENEMIGOS EN CAMPO",10,y,11,C_MUTED); y+=14;
        NodoEnemigo*ne=lE.getPrimero(); int ci=0;
        while(ne&&ci<4){
            sprintf(buf,"[%d] %s v:%d p:%d",ne->dato.id,ne->dato.tipo.substr(0,5).c_str(),ne->dato.vida,ne->dato.posicion);
            DrawText(buf,10,y+ci*16,10,C_TEXT);
            ne=ne->siguiente; ci++;
        }
        if(lE.getTamanio()==0) DrawText("(vacio)",10,y,10,C_MUTED);

        // Mensaje
        if(msgTimer>0){
            DrawRectangle(0,SH-36,258,36,{20,30,50,230});
            DrawText(msg.substr(0,30).c_str(),8,SH-28,10,C_YELLOW);
        }

        // ---- Mapa ----
        for(int c=0;c<GCOLS;c++) for(int r=0;r<GROWS;r++){
            Color tile=enCamino(c,r)?C_PATH:((c+r)%2==0?C_T1:C_T2);
            DrawRectangle(MAP_X+c*CELL,MAP_Y+r*CELL,CELL,CELL,tile);
        }
        for(int c=0;c<=GCOLS;c++) DrawLine(MAP_X+c*CELL,0,MAP_X+c*CELL,MAP_H,{25,32,48,55});
        for(int r=0;r<=GROWS;r++) DrawLine(MAP_X,r*CELL,MAP_X+MAP_W,r*CELL,{25,32,48,55});

        // Camino
        for(int i=0;i<N_WP-1;i++){Vector2 a=wpPx(i),b=wpPx(i+1);DrawLineEx(a,b,3,{70,110,70,80});}

        // INICIO / BASE
        Vector2 ini=wpPx(0),fin=wpPx(N_WP-1);
        boxR((int)ini.x-28,(int)ini.y-12,56,24,{0,160,80,200});
        DrawText("INICIO",(int)ini.x-22,(int)ini.y-7,11,WHITE);
        boxR((int)fin.x-22,(int)fin.y-12,44,24,{180,30,30,200});
        DrawText("BASE",(int)fin.x-16,(int)fin.y-7,11,WHITE);

        // Torres visuales
        for(int i=0;i<nT;i++){
            float cx=MAP_X+torresV[i].col*CELL+CELL/2.0f;
            float cy=MAP_Y+torresV[i].row*CELL+CELL/2.0f;
            Color tc=TIPOS[torresV[i].tipo].col;
            DrawCircleLines((int)cx,(int)cy,(int)(TIPOS[torresV[i].tipo].rango*CELL*1.3f),{tc.r,tc.g,tc.b,28});
            DrawCircle((int)cx,(int)cy,(int)(CELL*0.44f),{18,24,38,255});
            DrawCircle((int)cx,(int)cy,(int)(CELL*0.28f),tc);
            char b2[4]; sprintf(b2,"%d",torresV[i].id);
            DrawText(b2,(int)cx-4,(int)cy+CELL/2+1,9,{tc.r,tc.g,tc.b,200});
        }

        // Enemigos visuales
        for(int i=0;i<nE;i++){
            if(!enemigosV[i].vivo) continue;
            float r=CELL*0.32f;
            DrawCircle((int)enemigosV[i].x+2,(int)enemigosV[i].y+3,(int)r,{0,0,0,70});
            DrawCircle((int)enemigosV[i].x,(int)enemigosV[i].y,(int)r,enemigosV[i].col);
            DrawCircleLines((int)enemigosV[i].x,(int)enemigosV[i].y,(int)r,{255,255,255,45});
            char c2[2]={(char)enemigosV[i].tipo[0],0};
            DrawText(c2,(int)enemigosV[i].x-4,(int)enemigosV[i].y-5,11,WHITE);
            float bw=CELL,bh=4,bx=enemigosV[i].x-bw/2,by=enemigosV[i].y-r-8;
            DrawRectangle((int)bx,(int)by,(int)bw,(int)bh,{25,25,35,200});
            float pct=(float)max(0,enemigosV[i].hp)/enemigosV[i].mhp;
            Color hc=pct>0.6f?C_GREEN:pct>0.3f?C_YELLOW:C_RED;
            DrawRectangle((int)bx,(int)by,(int)(bw*pct),(int)bh,hc);
        }

        // Proyectiles
        for(int i=0;i<nP;i++) if(!projs[i].done){
            DrawCircle((int)projs[i].x,(int)projs[i].y,4,projs[i].col);
            DrawCircle((int)projs[i].x,(int)projs[i].y,2,WHITE);
        }

        // Partículas
        for(int i=0;i<nPt;i++){
            float a=parts[i].life/parts[i].ml;
            Color pc={parts[i].col.r,parts[i].col.g,parts[i].col.b,(unsigned char)(a*200)};
            DrawCircle((int)parts[i].x,(int)parts[i].y,max(1,(int)(3*a)),pc);
        }

        // Game Over / Victoria overlay
        if(estado.gameOver||estado.victoria){
            DrawRectangle(MAP_X,0,MAP_W,SH,{0,0,0,170});
            Color gc=estado.victoria?C_GREEN:C_RED;
            const char*gt=estado.victoria?"VICTORIA!":"GAME OVER";
            int tw=MeasureText(gt,54);
            int boxX=MAP_X+MAP_W/2-180,boxY=SH/2-80;
            DrawRectangleRounded({(float)boxX,(float)boxY,360,200},0.12f,8,C_PANEL);
            DrawRectangleLinesEx({(float)boxX,(float)boxY,360,200},2,gc);
            DrawText(gt,MAP_X+MAP_W/2-tw/2,boxY+16,54,gc);
            sprintf(buf,"Puntuacion: %d",estado.puntuacion);
            int tw2=MeasureText(buf,15);
            DrawText(buf,MAP_X+MAP_W/2-tw2/2,boxY+78,15,C_TEXT);
            int b1x=boxX+20,by2=boxY+110,bw2=150,bh2=44;
            Vector2 mp2=GetMousePosition();
            bool h1=mp2.x>b1x&&mp2.x<b1x+bw2&&mp2.y>by2&&mp2.y<by2+bh2;
            DrawRectangleRounded({(float)b1x,(float)by2,(float)bw2,(float)bh2},0.15f,8,h1?C_PANEL2:C_PANEL);
            DrawRectangleLinesEx({(float)b1x,(float)by2,(float)bw2,(float)bh2},h1?2:1,C_GREEN);
            int rtw=MeasureText("REINICIAR",13);
            DrawText("REINICIAR",b1x+bw2/2-rtw/2,by2+14,13,h1?C_GREEN:C_MUTED);
            int b2x=boxX+190;
            bool h2=mp2.x>b2x&&mp2.x<b2x+bw2&&mp2.y>by2&&mp2.y<by2+bh2;
            DrawRectangleRounded({(float)b2x,(float)by2,(float)bw2,(float)bh2},0.15f,8,h2?C_PANEL2:C_PANEL);
            DrawRectangleLinesEx({(float)b2x,(float)by2,(float)bw2,(float)bh2},h2?2:1,C_BLUE);
            int mtw=MeasureText("MENU",13);
            DrawText("MENU",b2x+bw2/2-mtw/2,by2+14,13,h2?C_BLUE:C_MUTED);
            if((h1&&IsMouseButtonPressed(MOUSE_LEFT_BUTTON))||IsKeyPressed(KEY_R)) resetJuego=true;
            if((h2&&IsMouseButtonPressed(MOUSE_LEFT_BUTTON))||IsKeyPressed(KEY_M)) irMenu=true;
        }
    }

    bool resetJuego=false;
    bool irMenu=false;

    void skipCurrentOleada(Estado& estado, ListaSecuencialTorres& lT, ListaDobleEnemigos& lE, ListaCircularOleadas& lO){
        const int maxSteps=1000;
        int steps=0;
        while(!estado.gameOver && (oleadaActiva || lE.getTamanio()>0 || spawneados < totalSpawn) && steps<maxSteps){
            update(1.0f/14.0f, estado, lT, lE, lO);
            steps++;
        }
    }

    void reset(Estado& estado){
        estado.vidas=3;estado.dinero=200;estado.puntuacion=0;estado.turno=0;
        estado.idTorre=1;estado.gameOver=false;estado.victoria=false;
        nT=0;nE=0;nP=0;nPt=0;
        oleadaActiva=false;oleadaIdx=0;spawneados=0;totalSpawn=0;spawnTimer=0;
        msg="Clic=torre | ESPACIO=oleada | T=turno | M=menu";msgTimer=4;
        resetJuego=false;irMenu=false;
    }
};
