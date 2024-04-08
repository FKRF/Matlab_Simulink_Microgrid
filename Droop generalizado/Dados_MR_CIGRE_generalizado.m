%% Código destinado para os parâmetros da simulação a microrrede
% Esse script deve ser executado antes de rodar o simulink MR_CIGRE_BASE_VX
%
% Esse script é destinado para a versão MR_CIGRE_BASE_V1, em que os
% equipamentos trocam de modo de operação após o ilhamento.
%
% 
% O seguinte contém as seguintes etapas:
% * DADOS PARA SIMULAÇÃO: local onde é possível alterar os tempos dos
% eventos de ilhamento e demais;
% * DADOS DE REFERÊNCIA PARA O SISTEMA CONECTADO: local onde é possível 
% alterar os valores de referência de geração e modos de operação;
% * DADOS DE ENTRADA DO SISTEMA -- NÃO ALTERAR: contém os valores de base
% de do sistema (é aconselhado não alterar esses valores);
% * DADOS DOS EQUIPAMENTOS E CARGAS: contém os parâmetros, ganhos e valores
% usados nos modelos dos equipamentos (é possível verificar e alterar os 
% parâmetros de cada equipamento em seu respectivo script).
%%
clc
clear all
close all
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% DADOS PARA SIMULAÇÃO
% Nesta etapa os tempos dos eventos da MR podem ser alterados
t_estab = 1;                                    % Auxiliar - Tempo para estabilizar o sistema [segundos]
% t_estab pode ser utilizado para auxiliar os demais eventos da MR; ou
% seja, é possível definir os demais tempos através do tempo de para
% estabilizar a MR, ou seja, para atingir o regime permanente no modo
% conectado.
 
t_deslig_rede = 10;                  % Tempo de ilhamento da rede Principal [segundos]
t_island_det =0;                          % Auxiliar - Tempo de detecção do ilhamento [segundos] - geralmente 160 ms
% t_island_det pode ser utilizado para auxiliar os demais eventos da MR; ou
% seja, é possível definir o tempo de ilhamento (tilha) através da soma do 
% tempo de detecção mais o tempo de ilhamento da rede principal.
 
tilha = 0.6;                                      % Tempo de ilhamento da MR [segundos]
t_atraso = 0;                                   % Atraso na troca de modos de operação para as fontes [segundos] - A troca de modos ocorre no tempo tilha + t_atraso
 
tsinc = 6;                                      % Tempo para sincronização [segundos]
t_sim = 2.0;                                      % Tempo de simulação [segundos]
 
% Obs.: o ilhamento da rede principal (t_deslig_rede) ilha o alimentador da
% MR; para ilhar a MR o tempo "tilha" deve ser usado
% Obs.: eventos com o tempo superior ao tempo de simulação (t_sim) não
% ocorrem, e podem ser desconsiderados;
%% Dados PARA EVENTOS DE TROCA DE CONTROLE
% Obs.: Essas variáveis são usadas somente quando o controle V/f é aplicado
% no modo conectado da MR, caso estiver usando o modo PQ no sistema
% conectado desconsiderar as variáveis.
 
t_delay_sw = 50e-3;                                 % Auxiliar - Tempo Delay para troca de modos 50ms
 
Sw_mode_bat1 = 0;                                   % Tempo Bateria 1 para troca de modo PQ para V/f quando a MR estiver conectada
Sw_mode_bat2 = 0;                                   % Tempo Bateria 2 para troca de modo PQ para V/f quando a MR estiver conectada
Sw_mode_bat3 = 0;                                   % Tempo Bateria 2 para troca de modo PQ para V/f quando a MR estiver conectada

t_Vdroop_PI_off_bat1 = tilha + t_delay_sw;  % Tempo Bateria 1 para troca de modo V/f com Q constante para V/f Droop
t_Vdroop_PI_off_bat2 = tilha + t_delay_sw;  % Tempo Bateria 2 para troca de modo V/f com Q constante para V/f Droop
t_Vdroop_PI_off_bat3 = tilha + t_delay_sw;  % Tempo Bateria 3 para troca de modo V/f com Q constante para V/f Droop

%% DADOS DE REFERÊNCIA PARA O SISTEMA CONECTADO
% Nesta etapa as referências dos geradores da MR podem ser alteradas
% BATERIA 1 - Barra R4
Pref_Bat1 = 0*1e3;          % Potência do banco de baterias [W]
Qref_Bat1 = 0*1e3;          % Potência do banco de baterias [var]
BAT1_CONT = 0;              % Modo de operação no ilhamento 0 - V/f; 1 - PQ
PQ_ref_time_bat1 = t_estab; %1.5; % Tempo de início das ref de P e Q [segundos]
% BATERIA 2 - Barra R9
Pref_Bat2 = 0*1e3;          % Potência do banco de baterias [W]
Qref_Bat2 = 0*1e3;          % Potência do banco de baterias [var]
BAT2_CONT = 0;              % Modo de operação no ilhamento 0 - V/f; 1 - PQ
PQ_ref_time_bat2 = t_estab;
% INTERMITENTES
    P_Solar1 = 21e3;     % Potencia da fonte Solar 1 [W] - Usar mÃºltiplos de 3kW! 
    P_Solar2 = 21e3;     % Potencia da fonte Solar 2 [W] - Usar mÃºltiplos de 3kW!
Qref_Solar = 0;             % Potência reativa do painel solar [var]
% CARGAS - Horário
Horario = 1;                % Variável para definir a geração das cargas [h]
%     1  2    3  4  5    6  7  8    9 10 11 12 13 14 15 16 17 18 19  20 21 22 23   24 25 EXP;  
% [27.5 25 22.5 20 15 22.5 30 40 42.5 45 50 50 55 60 60 55 50 65 85 100 90 75 57.5 30 5]/100;
% Horario - para alterar os eventos se deve alterar os eventos no script de
% carga (Dados_de_Cargas).
% Obs.: Os dados de cargas são para cargas não compostas (cargas sem
% hierarquia de prioridade)
%% DADOS DE ENTRADA DO SISTEMA -- NÃO ALTERAR
% Nesta etapa as bases da MR e dos equipamentos são apresentadas
% potência base do sistema
Sb = 200e3; % [W]
% potência base bateria 1
Sb_Bat1 = 45e3; % [W]
% potência base bateria 2
Sb_Bat2 = 45e3; % [W]
% potência base Solar
Sb_sol1 = 20e3; % [W]
% potência base eolica
Sb_sol2 = 20e3; % [W]
% potência base Gerador a Diesel
%tensão do sistema:
Vs1 = 20000;    % Base de tensão do sistema principal [V] 
Vs2 = 400;      % Base de tensão da MR [V] 
f0 = 50;        % Frequência de operação do sistema principal [Hz]
w0 = 2*pi*f0;   % Frequência angular de operação do sistema principal [rad/s]
%% DADOS DOS EQUIPAMENTOS E CARGAS
% Nesta etapa os parâmetros dos equipamentos da MR são apresentados
% carregados
% Parâmetros das cargas
Dados_de_Cargas
% Parâmetros das linhas (cabos) de transmissão
Dados_de_Linhas
% Parâmetros da bateria 1
Dados_de_Bat_1
% Parâmetros da bateria 2
Dados_de_Bat_2
% Parâmetros das fontes intermitentes (são os mesmos para as duas fontes)
Dados_de_Intermit
% Parâmetros do controle secundário (Obs.: controle não está presente no
% modelo base da MR).
Dados_de_Sec
% É possível fazer várias simulações em série, para tal leia o arquivo
% Automatico_XXX 

%% droop

rad = pi/180;

theta1 = 45*rad;
theta2 = 45*rad;

