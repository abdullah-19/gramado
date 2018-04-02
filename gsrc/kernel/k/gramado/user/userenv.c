/*
 * File: gramado\user\userenv.c 
 *
 * Descri��o:
 * User Environment Manager, (UEM).
 * Cria o ambiente do usu�rio, �rea de trabalho, onde o usu�rio interage 
 * com a m�quina (logo ap�s o logon). 
 *
 * MB - M�dulos inclu�dos no Kernel Base.
 *    
 * Tem usu�rios do tipo interactive e do tipo non_interactive.
 * * O ambiente gr�fico � criado para usu�rio tipo interactive.
 *
 * Rotinas de cria��o, inicializa��o, finaliza��o e configura��o 
 * do ambiente do usu�rio.
 * Inicia programas, troca backgrounds, cor padr�o de janelas ... etc.
 * Criar session, window stations, desktops. 
 * (usando as rotinas que est�o em outros m�dulos.)
 *
 * History:
 *     2015 - Created by Fred Nora.
 *     2016 - Revision.
 */

 
#include <kernel.h>


//User Environment Manager (UEM)

//
// Segue uma lista de opera��es que poder�o ser realizadas para configura��o
// do ambiente gr�fico do usu�rio. Obs: Nesse m�dulo, apenas chamaremos
// algumas fun��es primitivas. Todo o trabalho maior deve ficar para os
// servidores em user mode.
//

#define UEM_NULL 0
#define UEM_CHANGE_BACKGROUND 1
#define UEM_CHANGE_LOGO       2
#define UEM_CHANGE_COLORS     3
//...


//Vari�veis internas.
//int userenvironmentStatus;
//int userenvironmentError;
//...


//Internas.
void config_user();


/*
 *********************************************************************
 * startUserEnvironment:
 *     Cria um ambiente com janelas para usu�rio do tipo interativo.
 *     Obs: Isso n�o � realmente o ambiente que o usu�rio vai usar, mas sim 
 * as janelas principais onde o aplicativo 'file manager' (explorer e taskbar) 
 * v�o criar juas janelas.
 * 
 * Mudar o nome para userenvironmentStart(int argc, char* argv[]).
 */
int startUserEnvironment(int argc, char* argv[])
{
    int Status = 0;

    //Cria as principais janelas que servir�o de base para 
    //a interface gr�fica.
    create_gui();

    //Inicializa.
    init_gui(); 

done:
    SetProcedure( (unsigned long) &system_procedure);
    return (int) Status;
};


/*
 * ShowUserInfo:
 *     Mostra informa��es sobre o usu�rio atual.
 */
void ShowUserInfo(int user_id)
{
	struct user_info_d *User;
	
	if(user_id < 0 || user_id >= USER_COUNT_MAX){
	    return;
	};  
	
	User = (void *) userList[user_id];
	if( (void*) User == NULL ){
	    printf("ShowUserInfo: Error\n");
        return;		
	}else{
	    
		printf("Name={%s} Id={%d} Type={%d} \n",User->name_address
		                                       ,User->userId
											   ,User->userType );		
	    return;
	};
	
	//...
	
// Done.
done:
    return;
};


/*
 * config_user:
 *     Abre o arquivo de configura��o de usu�rio. 
 *     No arquivo ou metafile, ter�o todas as informa��es
 * sobre o usu�rio e ser�o colocanas nas estruturas.
 *
 */
void config_user()
{
    userconfig_Status = 0;	
	
	//...
	
done:
    return;    //Ainda n�o implementada.
};


/*
 * CreateUser:
 *     Cria um usu�rio do computador.
 */
void *CreateUser(char *name, int type)
{
	int Index = 0;	
   	struct user_info_d *New;

	New = (void*) malloc( sizeof(struct user_info_d) ); 
	if( (void*) New == NULL ){
	    printf("user-userenv-CreateUser:");
	    die();
	}else{
	    
		New->name = (char *) name;      
	    New->name_address = (unsigned long) name;
	    New->userType = type;    
	
	    New->sessionId       = current_usersession;      //Session. 
	    New->windowstationId = current_windowstation;    //Window Station. (Desktop pool). 
	    New->desktopId       = current_desktop;          //Desktop.

	    //Inicializa tokens. (rever)
	    //New->k_token = KERNEL_TOKEN_NULL;
	    //New->e_token = EXECUTIVE_TOKEN_NULL;
	    //New->m_token = MICROKERNEL_TOKEN_NULL;
	    //New->h_token = HAL_TOKEN_NULL;
		
	    //...
	};		
	

	//Procurando uma entrada livre na lista.
	while(Index < USER_COUNT_MAX)
	{	
        if( (void *) userList[Index] == NULL )
		{
		    userList[Index] = (unsigned long) New;
	        New->userId = Index;    //User Id.     
		    goto done;
		};
        ++Index;
	};
//Fail: 
//Fim do loop. 
//N�o encontramos uma entrada livre.
fail:
    return NULL;
done:
    //printf("CreateUser: Done.\n"); 
	return (void *) New;			
};


/*
 * SetCurrentUserId:
 *     Configura o ID do usu�rio atual.  
 */
void SetCurrentUserId(int user_id)
{
	//Limits.
	if(user_id < 0 || user_id > USER_COUNT_MAX){
		printf("user-userenv-SetCurrentUserId:\n");
		return;
	};
    current_user = (int) user_id;
	return;
};


/*
 * GetCurrentUserId: 
 *     Pega o ID do usu�rio atual.
 */
int GetCurrentUserId(){
   return (int) current_user;
};


void SetCurrentGroupId(int group_id)
{
	//Limits.
	if(group_id < 0 || group_id > GROUP_COUNT_MAX){
		printf("SetCurrentGroupId:\n");
		return;
	};
    current_group = (int) group_id;
	return;
};


/*
 * GetCurrentGroupId: 
 *     Pega o GID do usu�rio atual.
 */
int GetCurrentGroupId(){
    return (int) current_group;
};


/*
 * UpdateUserInfo:
 *    Atualiza todas as informa��es de usu�rio.
 */					 
void UpdateUserInfo( struct user_info_d *user, 
                     int id, 
					 char *name, 
					 int type, 
					 int user_session_id, 
					 int window_station_id,
					 int desktop_id )
{  
	if( (void*) user == NULL ){
        return;
	}else{	
	    
		user->userId = (int) id;                      //Id.     
		user->name = (char *) name;                   //Name.
	    user->name_address = (unsigned long) name;    //Name.
		user->userType = type;                        //Type.
	    user->sessionId = user_session_id;            //Session.
	    user->windowstationId = window_station_id;    //Window Station.
        user->desktopId = desktop_id;                 //Desktop.
		//...
	};
	//Nothing.
done:
    return;
};


/*
 * init_user_info:
 *     Configurar informa��es sobre o usu�rio default.
 *
 * Obs: 
 *     Deve ser poss�vel configurar usu�rios independente se 
 *     a interface gr�fica est� habilitada ou n�o.
 *
 * Obs:
 *     Utiliza as informa��es do usu�rio ativo. 
 *     O usu�rio ativo pode ser trocado.
 *     pode ser criar e deletar usu�rios.
 */
void init_user_info()
{	 
	int Id = 0;
    int Index = 0;	
	
	//Initialize list.
	while(Index < USER_COUNT_MAX){
	    userList[Index] = (unsigned long) 0;
        ++Index;
	};	
	
	//Configurando a estrutura global.
	// Create default user. (default,interactive)
	DefaultUser = (void*) CreateUser(default_user_name, USER_TYPE_INTERACTIVE);
	if( (void*) DefaultUser == NULL ){
	    printf("init_user_info:");
	    die();
	}else{
		
		//Coloca no in�cio da lista.
		//userList[0] = (unsigned long) SystemUser;    //System.
		userList[1] = (unsigned long) DefaultUser;     //Default.
		userList[2] = (unsigned long) 0;               //0.
		//userList[3] = (unsigned long) 0;               //0.
		//...
		
		//Configura o usu�rio atual.
		Id = (int) DefaultUser->userId;
		SetCurrentUserId(Id);
		CurrentUser = (void *) DefaultUser;
		
		//Configura o grupo atual ao qual o usu�rio pertence.
		SetCurrentGroupId(0);
		
		//...
		
		//g_logged = (int) 1;
	};
	
    // Continua...??!!

done:
    printf("Done\n"); 
    return;
};


/*
 * init_user_environment_manager:
 *    Inicializa o User Environment Manager. (UEM). 
 */
int init_user_environment_manager(int argc, char *argv[]) 
{
	//...
	
    //g_module_uem_initialized = 1;
    userenvironmentStatus = 1;
	return (int) 0;
};


/*
 * Construtor
void userenvironmentUserEnvironment()
{}
*/

/*
int userenvironmentInit()
{}
*/

//
// End.
//

