/*
 * File: gui\messagebox.c
 *
 * Descri��o:
 *     Mostra um Message Box na tela.
 *     Faz parte do m�dulo Window Manager do tipo MB.
 *
 * @Criar uma flag que indique que o message box deve considerar a m�trica do navegador
 * para sosicionar o message box de acordo com as m�rgens da aba com foco de entrada.
 * como j� temos o argumento 'type', ele pode servir para isso, � s� criar um tipo que 
 * considere as estruturas da aba do navegador e incluir a rotina no switch.
 * O message box � uma janela dentro de outra janela, isso facilita as coisas..
 * � s� criar um message box dentro da aba do navegador, que tamb�m � uma janela.
 *
 * History:
 *     2015 - Created by Fred Nora.
 *     2017 - Revision.
 */

 
#include <kernel.h>


unsigned long MessageBoxProcedure( struct window_d *window, 
                                   int msg, 
								   unsigned long long1, 
								   unsigned long long2 );

								   
	
/*
void messageboxIn(int type, char *title, char *string);
void messageboxIn(int type, char *title, char *string)
{
	//@todo: melhorar esse filtro.
    if( (void*) gui->shellWindow == NULL ){
		return;
	};
	
	MessageBox( (struct window_d *) gui->shellWindow, (int) type, (char *) title, (char *) string );
    return;				 
};
*/	

/*
void messageboxOut(struct window_d *parent_window, int type, char *title, char *string);
void messageboxOut(struct window_d *parent_window, int type, char *title, char *string)
{	
	MessageBox( (struct window_d *) parent_window, (int) type, (char *) title, (char *) string );
    return;				 
};
*/	

								   
/*
 * MessageBox:
 *     Cria uma Window Frame e apresenta uma mensagem nela.
 *     
 */
void MessageBox( struct window_d *parent_window,  //Window frame handle.      
                 int type,                        //Message Box type.
				 char *title,                     //Title for the window frame.
				 char *string )                   //Message string.
{	
	//
	// Set up.
	//
	
	struct button_d *b1;  //ponteiro para a estrutura de bot�o.
	struct button_d *b2;  //ponteiro para a estrutura de bot�o.
	
	//Elementos.
	int Button = 0;
	int Icon = 0;
	//continua...
	
	//unsigned long x  = (unsigned long) (1*(800/3));	
	//unsigned long y  = (unsigned long) (1*(600/3));
    //unsigned long cx = (unsigned long) (1*(800/3));
    //unsigned long cy = (unsigned long) (1*(600/3));	

    //ret�ngulo abaixo do grid.
	
	//x
	unsigned long x  = (unsigned long) (800 -400); //deslocamento x
    unsigned long cx = (unsigned long) 320;        //largura   
	
	//y
	unsigned long y  = (unsigned long) (600 -400); //deslocamento y
    unsigned long cy = (unsigned long) 240; //altura	

	
	struct window_d *hWnd;    //Window.
	struct window_d *pWnd;    //Parent.
	struct window_d *bWnd;    //Button.

	
	//No GUI.
	if(VideoBlock.useGui != 1){
		printf("MessageBox: No GUI\n");
	    return;	
	};
	
	if( (void*) gui->screen == NULL ){
	    return;
	};
	
	// Parent.
	if( (void*) parent_window == NULL ){
	    pWnd = gui->screen;
	}else{
	    pWnd = (void*) parent_window;
	};
	
	
	
	//
	// *Importante: Checando se o esquema de cores est� funcionando.
	//
	
	if( (void*) CurrentColorScheme == NULL ){
		printf("MessageBox: CurrentColorScheme");
		refresh_screen();
		while(1){}
	}else{
		
		if( CurrentColorScheme->used != 1 || CurrentColorScheme->magic != 1234 ){
		    printf("MessageBox: CurrentColorScheme validation");
		    refresh_screen();
		    while(1){}			
		};
		//Nothing.
	};
	
	// Configurando as cores usadas no message box.
	
	unsigned long WindowColor;
	unsigned long WindowClientAreaColor;
	
	WindowColor           = CurrentColorScheme->elements[csiWindowBackground];  
	WindowClientAreaColor = CurrentColorScheme->elements[csiWindow];  	
	
	
	int desktopID; 
	desktopID = (int) get_current_desktop_id();
	
	
	//
	// Quando criamos um message box, criamos uma janela.
	// do tipo que tem bot�o. ??
	//
	
	
//creatingFrame:
	
	// Creating a Window Frame for the Message Box. 
	// Obs: N�o queremos muitos tipos, isso � desnecess�rio.
	
	//
	// @todo: Usar o esquema de cores padr�o.
	//
	
	switch(type)
	{
		//
		// 765432(D=1) ??
		//
				
	    // Com bot�o, considera o t�tulo.
	    case 1:
		    Button = 1;
	        hWnd = (void*) CreateWindow( 3, 0, 0, title, 
			                             x, y, cx, cy, 
										 pWnd, desktopID, WindowClientAreaColor, WindowColor); 
		    break;
			
		// Sem bot�o, considera o t�tulo.	
	    case 2:
		    Button = 0;
	        hWnd = (void*) CreateWindow( 3, 0, 0, title, 
			                             x, y, cx, cy, 
										 pWnd, desktopID, WindowClientAreaColor, WindowColor); 
	        break;
			
		// Com bot�o, T�tulo de alerta.	
	    case 3:
	        Button = 1;
			hWnd = (void*) CreateWindow( 3, 0, 0, "Alert", 
			                             x, y, cx, cy, 
										 pWnd, desktopID, WindowClientAreaColor, WindowColor); 
	        break;
			
		//Com bot�o, t�tulo de mensagem do sistema.	
	    case 4:
		    Button = 1;
	        hWnd = (void*) CreateWindow( 3, 0, 0, "System Message", 
			                             x, y, cx, cy, 
										 pWnd, desktopID, WindowClientAreaColor, WindowColor); 
	        break;
			
		//Tipo negligenciado. Usamos o formato padr�o.	
		default:
		    Button = 1;
	        hWnd = (void*) CreateWindow( 3, 0, 0, "Error", 
			                             x, y, cx, cy, 
										 pWnd, desktopID, WindowClientAreaColor, WindowColor); 
		    break;
	};
	
	//
    // Register.
	//
	
	if((void*) hWnd == NULL){
	    printf("MessageBox:");
		refresh_screen();
	    while(1){};
	}else{   
		
		RegisterWindow(hWnd);
        set_active_window(hWnd);
		SetFocus(hWnd);  	 
		//...
	};
	
	//
	// Icon
	//
	//if ( Icon == 1){};
	
	//
    // String. (Uma string dentro da client area.)
    //
	
	//@todo: Calcular o tamanho da string.
	//size_t S;
	//S = (size_t) strlen( (const char*) string);
	
d_string:
	draw_text( hWnd, 1*(cx/16), (cy-16), COLOR_WINDOWTEXT, string);

	//
    // Button: 
	//    (Um bot�o dentro da client area.)
    //    @todo: � preciso criar um bot�o do tipo janela.
	//           para colocar foco na janela.
	//
	//
	//
d_button:	
    if(Button == 1)
	{
		//@todo: Criar uma janela do tipo bot�o para setar o foco.
        //       Pegar o tamanho da string para definir o tamanho do bot�o.
        b1 = (void*) draw_button( hWnd, "Close", 1, (cx-56-8), (cy-24-8), 56, 24, COLOR_BUTTONFACE); 
		if((void*) b1 == NULL ){
			printf("button 1 fail\n");
		}
		
		//b2 = (void*) draw_button( hWnd, "Close", 1, 12*(cx/16), 8*(cy/16), 48, 24, COLOR_BLACK);		
		//if((void*) b2 == NULL ){
		//	printf("button 2 fail\n");
		//}
		
	    //   
	    // @todo: FOCO NO BOT�O !!!
	    //
		
		//SetFocus(??);
	};
	
/*
    ** Outra maneira de cria um bot�o ***
    ** Obs: Retorna o haldle da janela **
    ** Obs: Precisamos de um handle para setar o foco. ***
	** e o bot�o ter� o message box como janela m�e.
	
        bWnd = CreateWindow( 11, 0, 0, "Close", 
		                     cx-102, y+2, 100, 24, 
							 hWnd, 0, 0, 0);
							 
	    if( (void*) bWnd == NULL )
	    {
	        printf("MessageBox error: Button Struct.\n");
		    refresh_screen();
	        while(1){};
	    }
	    else
	    {
		    SetFocus(bWnd);   //o bot�o tem o foco de entrada.
			RegisterWindow(bWnd); 
	    };	
*/		
	
	
	//
	// Continua ...
	//
	
	CurrentWindow = (void*) hWnd;
	SetProcedure( (unsigned long) &MessageBoxProcedure );	
	
done:
    //SetFocus(hWnd);		
    refresh_screen();		   
    return;
};


/*
 * MessageBoxProcedure:
 *     O procedimento padr�o de message box.
 */																
unsigned long 
MessageBoxProcedure( struct window_d *window, 
                     int msg, 
   				     unsigned long long1, 
					 unsigned long long2 )
{
    switch(msg)
	{
        case MSG_KEYDOWN:
            switch(long1)
            {
                case VK_ESCAPE:	
				    SetProcedure( (unsigned long) &system_procedure);
                    return (unsigned long) 0;				   
				    break;
				   
                default:
				    return (unsigned long) 0;
                    break; 
            };
        break;
	
        case MSG_SYSKEYDOWN:                 
            switch(long1)	       
            {	
				//Test.
				case VK_F1:
				    //DestroyWindow(window);
                    break;
					
                case VK_F2:
				    //Nothing.
					break;
									
				default:
				    return (unsigned long) 0;
				    break;
		    };              
        break;
		
	    case MSG_SYSKEYUP:
		   return (unsigned long) 0; 
           break;
		
        //@todo case command .. button ??
		
		default:
		    return (unsigned long) 0;
            break;		
	};
	
	
done:
	//Refresh screen. 
	if(VideoBlock.useGui == 1){
	    refresh_screen();
	};	
	return (unsigned long) 0;
};

/*
?? Pra qu� ??
int messageboxInit()
{}
*/

//
// End.
//

