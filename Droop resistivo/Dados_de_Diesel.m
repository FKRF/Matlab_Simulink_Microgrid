%% Dados de entrada Gerador a Diesel -- Não Alterar
% neste script é apresentado os parâmetros utilizados no gerador a diesel
% parâmetros retirados de UCI224D - Winding 311 STAMFORD Technical Data Sheet
% Utiliza como AVR AS540 - STAMFORD (modelo IEEE AC5A)
%% Dados do motor elétrico
Reactances_ger = [2.489 0.127 0.123 1.422 0.159 0.071];             % Reatancia do gerador [p.u.] - [Xd  Xd'   Xd''  Xq  Xq''  Xl]
ConsT_ger = [0.028 0.003 0.003];                                    % Constante de tempo do gerador [s] - [ Td'  Td''  Tq'' ]
Rs_ger = 0.03207;                                                   % Resistencia do estator (p.u.)
Rotor_ger = [0.117224 0.02842 4];                                   % Dados do rotor [inércia[s], fator de fricção [p.u.], número de polos]; 0.02842
Vger = 416;                                                         % Tensão Base do Gerador [V]
InCo_ger = [0 -123.386 0.874667 0.874667 0.874667 -91.2325 -211.232 28.7675 2.34948];                         % Condição inicial; 
%% dados do regulador de velocidade 
% OBS: Atualizar os nomes das variáveis caso alterar o modelo de regulador de velocidade
% É utilizado um atraso de transporte para representar a máquina primária, 
% os parâmetros foram ajustado de forma heurística
 
wref_gd = 1;                % Referência de frequência para o controle droop [p.u.]
 
Gain_Control_Ger = 0.05;    % Ganho proporcional do controlador PID para o controle PQ [p.u.]
Gain_Control_Ger_I = 0.1;   % Ganho integrativo do controlador PID para o controle PQ [p.u.]
Gain_Control_Ger_D = 0.00;  % Ganho derivativo do controlador PID para o controle PQ [p.u.]
 
T_atuador_ger1 = 0.22;      % Constante de tempo do atuador [s] - zero T4
T_atuador_ger2 = 0.01;      % Constante de tempo do atuador [s] - polo T5
T_atuador_ger = 0.04;       % Constante de tempo do atuador [s] - polo T6
Gain_Atuador_ger = 35;      % Ganho do atuador [p.u.] - K
 
Tmim_Reg_ger = 0;           % Torque mínimo do saturador [p.u.]
Tmax_Reg_ger = [1.1];       % Torque máximo do saturador [p.u.]
Delay_Reg_ger = 0.02;       % Atraso de transporte - máquina primária [p.u.] -Td
 
T1_RV = 0.0005;             % Constante de tempo do controlador no modo V/f [s] - polo T1
T2_RV = 0.001;              % Constante de tempo do controlador no modo V/f [s] - polo T2
T3_RV = 0.038;              % Constante de tempo do controlador no modo V/f [s] - zero T3
 
Pmec0 = 0;                  % Dado potência mecânica inicial (powerflow) [p.u.]
%% Dados do sistema excitação (modelo IEEE AC5A)
% Os parâmetros foram ajustados de forma heurística
 
Vref_gd = 1*Vs2/Vger;       % Tensão de referência para o controle droop [p.u.] - Obs: a base de tensão do gerador é 416V, enquanto a base do sistema é de 380, logo o droop deve ser 1 p.u. na base do sistema 
 
Te_Exc_ger = 0.075;         % Constante de tempo da excitação [s]
Ke_Exc_ger = 1;             % Ganho da excitação [p.u.]
 
SeEfd1_ex = 0.86;           % Valor de referência para a função de saturação [p.u.]
Efd1_ex = 5.6;              % Valor de tensão para a função de saturação [p.u.]
SeEfd2_ex = 0.5;            % Valor de referência para a função de saturação [p.u.]
Efd2_ex = 4.4;
 
Tr_Exc_ger = 20e-3;         % Constante de tempo do filtro passa baixa de entrada [s]
 
Ka_Exc_ger = 1600;          % Ganho do regulador [p.u.]
Ta_Exc_ger = 0.03;          % Constante de tempo do regulador [s]
 
V_ex_min = -20;             % Tensão limite mínima para o regulador [p.u.]
V_ex_max = 20;              % Tensão limite máxima para o regulador [p.u.]
 
Kf_Exc_ger = 0.03;          % Ganho do filtro de amortecimento [p.u.]
Tf1_Exc_ger = 0.27;         % Constante de tempo do filtro de amortecimento [s] - polo
Tf2_Exc_ger = 0.27;         % Constante de tempo do filtro de amortecimento [s] - polo
Tf3_Exc_ger = 0.1;          % Constante de tempo do filtro de amortecimento [s] - zero
 
Kp_Exc_ger = 2;             % Ganho proporcional do controlador PID para o controle PQ [p.u.]
Ki_Exc_ger = 2;             % Ganho integrativo do controlador PID para o controle PQ [p.u.]
Kd_Exc_ger = 0.5;           % Ganho derivativo do controlador PID para o controle PQ [p.u.]
 
%% Dados do controle Droop
n_ger = 0.05;               % Estatismo para a tensão [%]
m_ger = 0.05;               % Estatismo para a frequência [%]
 
Tdr_rt = 0.4;               % Filtro saída do medidor de potência ativa [s]
Tdr_rv = 0.4;               % Filtro saída do medidor de potência reativa [s]