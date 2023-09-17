#include "config.h"
#include "GLCD.h"
#include "splash.h"
#include <string.h>
#include <xc.h>
#define _XTAL_FREQ 8000000  

//strings
const char *s1 = "Jordi Bru:\n";
const char *punts = "Points: \n";
const char *flecha = " --> \n";

//variables globales
int decs=0;
int score = 0;
int vidas = 3;
int abans = 0;
int pos = 5;

int anotado = 0;

//nivel de dificultad
int dificultat = 10; //standard easy
int aux_mode = 10;
int easy = 10;
int medium = 7;
int hard = 4;
int insane = 1;

//posiciones de los "cuerpos" del juego
int vpos=0;
int h_pic =1; //para abajo, mas num
int x_pic = 2; //posicion horizontal

int v[8] = {2,5,18,11,7,19,15,10}; //vector de las posiciones de donde cae el pic.
int vhits[8] = {0,0,0,0,0,0,0,0}; //vector para saber si en cada ronda si ha sumado punto o restado una vida.



//printar text:
void writeTxt(byte page, byte y, char * s) {
	int i=0;
	while (*s!='\n' && *s!='\0') { 
		putchGLCD(page, y+i+1, *(s++));
		i++;
	}
}	

//printar pel terminal el meu nom:
void print_name(){ 
   for(int i = 0; i < 10; i++){
      TXREG1 = s1[i];
      while (!TXSTAbits.TRMT);
   } 
}

 //printar pel terminal els punts cada cop que es guanyin:
void print_term_punts(){
	for(int i = 0; i < 5; i++){
		TXREG1 = flecha[i];
		while (!TXSTAbits.TRMT);
	} 
    for(int i = 0; i < 8; ++i){
		TXREG1 = punts[i];
		while (!TXSTAbits.TRMT);
    }
    char c = score + '0';
    TXREG1 = c;
    while (!TXSTAbits.TRMT){} 
}

//printa el PIC que cau:
void print_pic(){
      if(pos == x_pic || pos+1 == x_pic) clearGLCD(1,4,x_pic*5,(x_pic+1)*5); 
	  else clearGLCD(1,5,x_pic*5,(x_pic+1)*5); 
  
      if(((pos == x_pic || pos+1 == x_pic) && (h_pic >= 5)) || anotado == 1){}
      else putchGLCD(h_pic,x_pic,',');
}

//printa el titol de benvinguda:
void print_title(){
    int pos = 0;
      for(int i = 0; i <= 7; ++i){
		for(int j = 0; j <= 127; ++j){
			writeByte(i,j,bitmap[pos]);
			pos++;
		}
      }
}

//printa el personatje que controlem:
void print_player(){
    clearGLCD(5,6,0,127);
    putchGLCD(5,pos,'%');
    putchGLCD(5,pos+1,'&');
    putchGLCD(6,pos,'(');  
    putchGLCD(6,pos+1,')');  
}

//printa el terra de la partida:
void print_floor(){
   for(int i = 0; i < 26; i++){
	 putchGLCD(7,i,'-');
   }
}

//printa la puntuacio en el glcd i al terminal:
void print_score(){
   clearGLCD(0,0,45,63);
   writeTxt(0,0,"Points:");
   writeNum(0,9,score);
   print_term_punts();
}

//printa les vides que queden en el glcd:
void print_vidas(){
   clearGLCD(0,0,100,127);
   writeTxt(0,12,"Lives:");
   if (vidas > 0){
      putchGLCD(0,20,'+');
      if(vidas > 1){
		putchGLCD(0,22,'+');
		if(vidas > 2){
			putchGLCD(0,24,'+');   
		}
      }     
   }
}

//funcio que acaba el joc en derrota:
void perdedor(){		
      if(vidas <= 0 ){
	    TMR0ON = 0;
		__delay_ms(800);
		clearGLCD(0, 7, 0, 127);
        while(1){
	    writeTxt(3,3,"GAME OVER");
	    writeTxt(4,3,"TRY AGAIN LOSER");
		writeTxt(6,3,"Final Score: ");
		writeNum(6,19,score);
		}
      }
}

//funcio que acaba el joc en victoria:
void ganador(){
   if ( score == 5){
	  TMR0ON = 0;
	  __delay_ms(800);
      clearGLCD(0, 7, 0, 127);
      while(1){
		writeTxt(3,5,"CONGRATZ!");
		writeTxt(4,5,"YOU WIN"); 
      }
   }
}

//funcio que consulta el POT-HG per saber la dificultat pel seguent PIC:
void consulta_dificultat(){
   ADCON0bits.GO = 1; // Demanem una conversio
   while(ADCON0bits.GO){} // Espera activa a que ens confirmi la lectura
   int valor = (ADRESH*256 + ADRESL);
   if (valor <= 256) aux_mode = insane;
   else if(valor > 256 && valor <= 512) aux_mode = hard;
   else if(valor > 512 && valor <= 768) aux_mode = medium;
   else aux_mode = easy;
}

//funcio del timer que en cada decima de segon que regula la velocitat en que cau el PIC:
void tic(){
      if( decs == dificultat ){
		decs=0;
		print_pic();
		if(h_pic+1<7) h_pic++;
		else{
			h_pic = 1;
			dificultat = aux_mode;
			clearGLCD(h_pic,5,0,127);
			print_player();
		    vpos++;
		    x_pic = v[vpos];
			anotado = 0;
			}
		}
      else if(decs >= dificultat) decs = 0;
      ganador();
      perdedor();	 
} 

//printa totes les pantalles del glcd de benvinguda:
void welcome_back(){
   print_title();
   __delay_ms(3000);
   print_name();
   clearGLCD(0, 7, 0, 127);
   writeTxt(1,0,"Instrucciones:");
   writeTxt(3,0,"Primero EASY MODE.");
   writeTxt(4,0,"WIN: Recoge 5 PIC's.");
   writeTxt(5,0,"LOSE: Deja caer 3 PIC's.");
   __delay_ms(3000);
   clearGLCD(0, 7, 0, 127);
   writeTxt(3,2,"El juego empieza en:");
   writeNum(5,11,3);
   __delay_ms(1000);
   clearGLCD(4, 4, 0, 127);
   writeNum(5,11,2);
   __delay_ms(1000);
   clearGLCD(4, 4, 0, 127);
   writeNum(5,11,1);
   __delay_ms(1000);
   clearGLCD(4, 4, 0, 127);
   writeNum(5,11,0);
   __delay_ms(1000);
   clearGLCD(0, 7, 0, 127);
}

//interrupcio pel timer, calculat per a que sigui cada decima de segon i aixi poder jugar mes facil amb la dificultat:
void interrupt timer(void){
   if(TMR0IE && TMR0IF){
      TMR0H = 0xE7;
      TMR0L = 0x95;
      TMR0IF = 0;
      decs++;
   }
}

//configuracio del PIC:
void configPIC(){
   ANSELA = 1;
   ANSELB = 0;
   ANSELD = 0;
   PORTA = 0;
   TRISA = 0xFF;
  
   // A/D 
   TRISA = 0x07; //Configurem pin 0 com a entrada
   ADCON0 = 0x01; //Activem el AD
   ADCON1 = 0x01; //AN0 com a analogics i la resta com a digitals
   ADCON2 = 0xA1; //Left justified, TAD = 8/Fosc, Tacq = 8 Tad
    
   //Timers
   INTCON = 0xA0; 	// Interrupciones activadas, TMR0IE = 1. Activa las interrupciones por overflow de Timer0
   RCON = 0;		// IPEN = 0
   T0CON = 0x04;	// Configuracion Timer0 (Prescaler 1:32)
   TMR0H = 0xE7;	// Contador inicializado a 59285 
   TMR0L = 0x95;
   
   //Virtual terminal linea serie
   TRISCbits.RC6 = 1;
   TRISCbits.RC7 = 1;
   
   TXSTAbits.TXEN = 1; //enable Tx   
   TXSTAbits.BRGH = 1; //high baud rate
   TXSTAbits.SYNC = 0; //asinÂ­crono
   TXSTAbits.TX9 = 0; //8 or 9 bits
 
   RCSTAbits.CREN = 1; //enable Rx
   RCSTAbits.SPEN = 1; //serial port enable
   
   BAUDCONbits.BRG16 = 1; //16-bit Baud Rate Generator is used (SPBRGx)
   SPBRG1 = 16; //baud rate
}

//main
void main(void){
   GLCDinit();
   clearGLCD(0, 7, 0, 127);
   setStartLine(0);
   configPIC();
   welcome_back();
   print_floor();
   print_player();
   print_score();
   print_vidas();
   TMR0ON = 1;
   while (1){
      tic();
      print_floor();
      consulta_dificultat(); 

	  //consulta constante de los botones 
      if((PORTAbits.RA1 == 0 || PORTAbits.RA2 == 0) && abans == 1){ 	   	 
		abans = 0;
		print_player();
		print_pic();
      }
      else if(PORTAbits.RA1 == 1 && abans == 0){
		abans = 1;
		pos--;
      }
      else if(PORTAbits.RA2 == 1 && abans == 0){
		abans = 1;
		pos++;
      } 
   	  
	  //consulta constante de la suma de puntos o resta de vidas
	  if (h_pic >= 6){
	    if((pos == x_pic || pos+1 == x_pic) && vhits[vpos] == 0){
		    score = score + 1;
			anotado = 1;
		    print_score();
			vhits[vpos] = 1;
	    }
	  }
	  else if(vpos != 0 && h_pic == 1){
		if(vhits[vpos-1] == 0){
			vidas = vidas - 1;
			vhits[vpos-1] = -1;
			print_vidas();
		}
	  }
     }
}   

      