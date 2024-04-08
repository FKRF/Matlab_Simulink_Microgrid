%% C�digo destinado para os par�metros da simula��o a microrrede
% Esse script deve ser executado antes de rodar o simulink MR_CIGRE_BASE_VX
%
% Esse script � destinado para a vers�o MR_CIGRE_BASE_V1, em que os
% equipamentos trocam de modo de opera��o ap�s o ilhamento.
%
% 
% O seguinte cont�m as seguintes etapas:
% * DADOS PARA SIMULA��O: local onde � poss�vel alterar os tempos dos
% eventos de ilhamento e demais;
% * DADOS DE REFER�NCIA PARA O SISTEMA CONECTADO: local onde � poss�vel 
% alterar os valores de refer�ncia de gera��o e modos de opera��o;
% * DADOS DE ENTRADA DO SISTEMA -- N�O ALTERAR: cont�m os valores de base
% de do sistema (� aconselhado n�o alterar esses valores);
% * DADOS DOS EQUIPAMENTOS E CARGAS: cont�m os par�metros, ganhos e valores
% usados nos modelos dos equipamentos (� poss�vel verificar e alterar os 
% par�metros de cada equipamento em seu respectivo script).
%%
clc
clear all
close all
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% DADOS PARA SIMULA��O
% Nesta etapa os tempos dos eventos da MR podem ser alterados
t_estab = 1;                                    % Auxiliar - Tempo para estabilizar o sistema [segundos]
% t_estab pode ser utilizado para auxiliar os demais eventos da MR; ou
% seja, � poss�vel definir os demais tempos atrav�s do tempo de para
% estabilizar a MR, ou seja, para atingir o regime permanente no modo
% conectado.
 
t_deslig_rede = 10;                  % Tempo de ilhamento da rede Principal [segundos]
t_island_det =0;                          % Auxiliar - Tempo de detec��o do ilhamento [segundos] - geralmente 160 ms
% t_island_det pode ser utilizado para auxiliar os demais eventos da MR; ou
% seja, � poss�vel definir o tempo de ilhamento (tilha) atrav�s da soma do 
% tempo de detec��o mais o tempo de ilhamento da rede principal.
 
tilha = 0.6;                                      % Tempo de ilhamento da MR [segundos]
t_atraso = 0;                                   % Atraso na troca de modos de opera��o para as fontes [segundos] - A troca de modos ocorre no tempo tilha + t_atraso
 
tsinc = 6;                                      % Tempo para sincroniza��o [segundos]
t_sim = 2.0;                                      % Tempo de simula��o [segundos]
 
% Obs.: o ilhamento da rede principal (t_deslig_rede) ilha o alimentador da
% MR; para ilhar a MR o tempo "tilha" deve ser usado
% Obs.: eventos com o tempo superior ao tempo de simula��o (t_sim) n�o
% ocorrem, e podem ser desconsiderados;
%% Dados PARA EVENTOS DE TROCA DE CONTROLE
% Obs.: Essas vari�veis s�o usadas somente quando o controle V/f � aplicado
% no modo conectado da MR, caso estiver usando o modo PQ no sistema
% conectado desconsiderar as vari�veis.
 
t_delay_sw = 50e-3;                                 % Auxiliar - Tempo Delay para troca de modos 50ms
 
Sw_mode_bat1 = 0;                                   % Tempo Bateria 1 para troca de modo PQ para V/f quando a MR estiver conectada
Sw_mode_bat2 = 0;                                   % Tempo Bateria 2 para troca de modo PQ para V/f quando a MR estiver conectada
Sw_mode_bat3 = 0;                                   % Tempo Bateria 2 para troca de modo PQ para V/f quando a MR estiver conectada

t_Vdroop_PI_off_bat1 = tilha + t_delay_sw;  % Tempo Bateria 1 para troca de modo V/f com Q constante para V/f Droop
t_Vdroop_PI_off_bat2 = tilha + t_delay_sw;  % Tempo Bateria 2 para troca de modo V/f com Q constante para V/f Droop
t_Vdroop_PI_off_bat3 = tilha + t_delay_sw;  % Tempo Bateria 3 para troca de modo V/f com Q constante para V/f Droop

%% DADOS DE REFER�NCIA PARA O SISTEMA CONECTADO
% Nesta etapa as refer�ncias dos geradores da MR podem ser alteradas
% BATERIA 1 - Barra R4
Pref_Bat1 = 0*1e3;          % Pot�ncia do banco de baterias [W]
Qref_Bat1 = 0*1e3;          % Pot�ncia do banco de baterias [var]
BAT1_CONT = 0;              % Modo de opera��o no ilhamento 0 - V/f; 1 - PQ
PQ_ref_time_bat1 = t_estab; %1.5; % Tempo de in�cio das ref de P e Q [segundos]
% BATERIA 2 - Barra R9
Pref_Bat2 = 0*1e3;          % Pot�ncia do banco de baterias [W]
Qref_Bat2 = 0*1e3;          % Pot�ncia do banco de baterias [var]
BAT2_CONT = 0;              % Modo de opera��o no ilhamento 0 - V/f; 1 - PQ
PQ_ref_time_bat2 = t_estab;
% INTERMITENTES
    P_Solar1 = 21e3;     % Potencia da fonte Solar 1 [W] - Usar múltiplos de 3kW! 
    P_Solar2 = 21e3;     % Potencia da fonte Solar 2 [W] - Usar múltiplos de 3kW!
Qref_Solar = 0;             % Pot�ncia reativa do painel solar [var]
% CARGAS - Hor�rio
Horario = 1;                % Vari�vel para definir a gera��o das cargas [h]
%     1  2    3  4  5    6  7  8    9 10 11 12 13 14 15 16 17 18 19  20 21 22 23   24 25 EXP;  
% [27.5 25 22.5 20 15 22.5 30 40 42.5 45 50 50 55 60 60 55 50 65 85 100 90 75 57.5 30 5]/100;
% Horario - para alterar os eventos se deve alterar os eventos no script de
% carga (Dados_de_Cargas).
% Obs.: Os dados de cargas s�o para cargas n�o compostas (cargas sem
% hierarquia de prioridade)
%% DADOS DE ENTRADA DO SISTEMA -- N�O ALTERAR
% Nesta etapa as bases da MR e dos equipamentos s�o apresentadas
% pot�ncia base do sistema
Sb = 200e3; % [W]
% pot�ncia base bateria 1
Sb_Bat1 = 45e3; % [W]
% pot�ncia base bateria 2
Sb_Bat2 = 45e3; % [W]
% pot�ncia base Solar
Sb_sol1 = 20e3; % [W]
% pot�ncia base eolica
Sb_sol2 = 20e3; % [W]
% pot�ncia base Gerador a Diesel
%tens�o do sistema:
Vs1 = 20000;    % Base de tens�o do sistema principal [V] 
Vs2 = 400;      % Base de tens�o da MR [V] 
f0 = 50;        % Frequ�ncia de opera��o do sistema principal [Hz]
w0 = 2*pi*f0;   % Frequ�ncia angular de opera��o do sistema principal [rad/s]
%% DADOS DOS EQUIPAMENTOS E CARGAS
% Nesta etapa os par�metros dos equipamentos da MR s�o apresentados
% carregados
% Par�metros das cargas
Dados_de_Cargas
% Par�metros das linhas (cabos) de transmiss�o
Dados_de_Linhas
% Par�metros da bateria 1
Dados_de_Bat_1
% Par�metros da bateria 2
Dados_de_Bat_2
% Par�metros das fontes intermitentes (s�o os mesmos para as duas fontes)
Dados_de_Intermit
% Par�metros do controle secund�rio (Obs.: controle n�o est� presente no
% modelo base da MR).
Dados_de_Sec
% � poss�vel fazer v�rias simula��es em s�rie, para tal leia o arquivo
% Automatico_XXX 

%% droop

rad = pi/180;

theta1 = 45*rad;
theta2 = 45*rad;

