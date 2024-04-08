%% Rodar Simulink e salvar vari�veis 
% Procedimentos para rodar v�rios casos automaticamente:
% 1) definir a pasta (diret�rio) em que ser� salvo os arquivos (a vari�vel
% PASTA neste script tem como objetivo definir o diret�rio) 
% 2) Definir os N eventos que se deseja realizar, e criar vari�veis com
% esses eventos (EX: 6 eventos com a vari�vel "HorarioV = [1 2 3 4 5 6];");
% 3) Definir as condi��es da MR, tipo de controles, cargas, etc...
% 4) Criar um la�o de repeti��o (For ou while) o qual dever� executar o
% la�o N vezes;
% 5) Dentro do la�o atribuir as vari�veis de simula��o com o respectivo
% valor da vari�vel de evento criadas no passo 2 (EX: Horario =
% HorarioV(i) - i � a vari�vel que indica o numero do evento);
% 6) Em sequ�ncia definir uma vari�vel que gera um novo diret�rio para cada
% simula��o (EX: Dir = [PASTA NAME], em que NAME deve ser uma vari�vel que
% mude de nome a cada intera��o, para que cada diret�rio seja diferente);
% 7) Em sequ�ncia incluir o comando "mkdir(DIR);" para gerar o diret�rio
% automaticamente;
% 8) Em sequ�ncia incluir o comando "sim('MR_CIGRE_BASE_V1.slx');", assim o
% simulink da MR ser� executado automaticamente;
% 9) Em sequ�ncia usar incluir os comandos para salvar as vari�veis da MR
% "save([DIR 'MGCI.mat'],'MGCC_VI')";
% 10) Finalizar o la�o de repeti��o.
% 
% Obs.: � poss�vel gerar gr�ficos p�s uma simula��o ou p�s todas as
% simula��es.
%% SIMULAR EVENTOS
%% 1) definir a pasta (diret�rio)
PASTA = ['Artigo_1\']; % Definir o diret�rio das simula��es 
% Obs.: Deve ser colocado "\" no fim do nome do diret�rio, do contr�rio n�o
% funcionara a l�gica proposta
 
%% 2) Definir os N eventos que se deseja realizar
N = 6;                              % N�mero de eventos 
HorarioV = [1 2 3 1 2 3];           % Vetor para vari�vel do hor�rio
Pref_SolarV = [0 0 0 20 20 20]*1e3; % Vetor para vari�vel de pot�ncia ativa do painel solar (PV)
 
%% 3) Definir as condi��es da MR
Qref_Solar = 0;                     % Pot�ncia reativa do painel solar
t_sim = 4;                          % tempo de ilhamento
% Obs.: Se n�o for definidas as vari�veis nesta etapa, ser�o selecionadas  
% as condi��es apresentadas nos demais scripts ser�o selecionadas. No exemplo
% mostrado a pot�ncia reativa do PV ser� mantida em 0, e o tempo de
% ilhamento ocorrera em 4 segundos de simula��o, independentes dos valores
% apresentados no script Dados_MR_CIGRE_Modelo. 
 
%% 4) Criar um la�o de repeti��o
for i=1:N
    %% 5) Atribuir as vari�veis de simula��o
    Horario = HorarioV(i);          % Hor�rio de carga
    Pref_Solar = Pref_SolarV(i);    % Pot�ncia ativa gerada pelo PV
    % Obs.: a cada la�o de repeti��o as vari�veis iram ganhar novos valores
    
    %% 6) Definir uma vari�vel que gera um novo diret�rio
    NAME = ['H_' num2str(Horario) 'PV_P_' num2str(Pref_Solar) '\'];     % nome do novo diret�rio
    Dir = [PASTA NAME];                                                 % Vari�vel de diret�rio
    % Obs.: � poss�vel usar uma vari�vel de vetor com caracteres para gerar o
    % nome.
    %Obs. 2: Usar \ no fim da vari�vel NAME... 
    
    %% 7) Gerar o diret�rio automaticamente
    mkdir(DIR);                     % Fun��o para gerar o diret�rio
    
    %% 8) Executar simulink automaticamente
    sim('MR_CIGRE_BASE_V1.slx');
    % Obs.: Caso queira executar outro modelo da MR altere o nome do
    % arquivo para o desejado
    
    %% 9) Salvar as vari�veis da MR    
    save([DIR 'GERA.mat'],'GeD')
    save([DIR 'BAT1.mat'],'Bt1')
    save([DIR 'BAT2.mat'],'Bt2')
    
    save([DIR 'SOLA.mat'],'Sol')
    save([DIR 'EOLI.mat'],'Eol')
     
    save([DIR 'LO11.mat'],'R11')
    save([DIR 'LO15.mat'],'R15')
    save([DIR 'LO16.mat'],'R16')
    save([DIR 'LO17.mat'],'R17')
    save([DIR 'LO18.mat'],'R18')
    % Obs.: No comando save, a primeira entrada indica o nome da vari�vel,
    % e o segundo o nome da vari�vel no Simulink.
    
    %% 10) Finalizar o la�o de repeti��o
end
