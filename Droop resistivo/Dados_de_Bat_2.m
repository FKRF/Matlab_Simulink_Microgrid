%% Dados de entrada do inversor (retirados de Bidram_2013) 2:
% neste script � apresentado os par�metros utilizados na bateria 2, com o
% controle dq0, assim como descritos em Yazdani 2010 (livro VOLTAGE-SOURCED
% CONVERTERS IN POWER SYSTEMS)
%% Calculo das correntes de base e m�ximas (1.5 vezes maior que a base)
Is_Bat2 = Sb_Bat2 / (sqrt(3)*Vs2); %Corrente nominal do inversor em RMS.
I_max_Bat2 = 1.5 * Is_Bat2 * sqrt(2); %Corrente m�xima do inversor em valor de pico.
%% Dados do filtro LRC
L_Bat2 = 1.35E-3;       % Reat�ncia s�rie do filtro [H]
R_Bat2 = 0.1;           % Resistencia s�rie do filtro [Ohms]
ron_Bat2 = 0.88E-3;     % Resistencia s�rie dos semicondutores [Ohms]
Cf_Bat2 = 5E-6;         % Capacit�ncia shunt do filtro [F]
%% Dados do conector
RC_Bat2=1e-16;          % Resistencia s�rie do conector [Ohms]
LC_Bat2=1e-16;          % Reat�ncia s�rie do filtro [H]
%% Dados do sistema DC:
Vdc_Bat2 = 700;          % Tens�o no lado DC do conversor [V-pico]
Vdc_Bat2 = 800;          % modificacao
Cdc_Bat2 = 19.24E-6/2;   % Capacit�ncia shunt do lado DC do conversor [F]
%% Dados do controlador de corrente:
ti_Bat2 = 0.5E-3;                       % Constante de tempo para o controle de corrente (entre 0.5 a 5 ms) [s]
Kp_Bat2 = 1;                            % Ganho proporcional [V/A]
Ki_Bat2 = 1200;                         % Ganho integral [V/A]
LPF_Ci_Bat2 = 5000;                     % Filtro sa�da do controlador de corrente [1/s]
%% Dados do controlador de tens�o:
z_Bat2 = 500;                           % M�ltiplo do ganho integrador [~]
Kv_Bat2 = 1;                            % Ganho s�rie (ele � o proporcional, porem multiplica M�ltiplo do ganho integrador para formar o ganho integrador) [A/V];
LPF_Cv_Bat2 = 3000;                     % Filtro sa�da do controlador de tens�o e pot�ncia [1/s]
K_iff_Bat2 = 0;                         % Ganho do la�o feed forward de corrente [~]
K_wup_Bat2 = 3/Kv_Bat2;                 % Ganho da realimenta��o anti windup [~]
%% Dados do droop bat:
n_Bat2 = 0.05;                          % Estatismo para a tens�o [%]
m_Bat2 = 0.05;                          % Estatismo para a frequ�ncia [%]

R_Virt_Bat2 =0;                         % Resistencia s�rie da imped�ncia virtual [Ohms]
L_Virt_Bat2 = 3.0E-3; %1E-3;                     % Reat�ncia s�rie da imped�ncia virtual [H]

LPF_P_Bat2 = 20;                        % Filtro sa�da do medidor de pot�ncia ativa [1/s]
LPF_Q_Bat2 = 20;                        % Filtro sa�da do medidor de pot�ncia reativa [1/s]
%% Configura tempo de in�cio e taxa da rampa de refer�ncia
PQ_ref_taxavar_bat2 = 0.5;              % Taxa da rampa de ref de P e Q [p.u./s]
PQ_ref_time_bat2 = t_estab;             % Tempo de aplica��o das refer�ncias