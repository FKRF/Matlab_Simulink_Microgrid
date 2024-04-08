%% Modelo da Bateria - célula de Ion-Litio %%

% Modelo: Gkountaras (2017) - Modeling Techniques and Control Strategies
% Dados de uma célula de bateria %
Voc_cell = 3.6; % [V] tensão da célula em SoC de 50% e 25°C.
I_cell = 5; % [Ah] capacidade da célula
Re_cell = 50e-3; % [ohm] resistência interna da célula
Rct_cell = 80e-3; % [ohm] resistência de transf. de carga da célula
Rw_cell = 54e-3; % [ohm] resistência de Warbung da célula
Rb_cell = Re_cell + Rct_cell + Rw_cell; % [ohm] resistência total da célula

% Matriz de células %
N_cell_serie = 50; %Número de células em série
N_string_cell = 60; %Número de strings de células em paralelo

% Modelo Bateria a partir de uma matriz de célula %
Voc_bat1 = N_cell_serie * Voc_cell;  % 180Vcc tensão circuito aberto (OC)
Rb_bat1 = (N_cell_serie * Rb_cell) / N_string_cell;  % 0.23 ohms
Inom_bat1 =  I_cell * N_string_cell; % 300 Ah

%% Dados de entrada do inversor (VSI) (retirados de Bidram_2013) 1: -- N�o Alterar

Is_bat1 = Sb_Bat1 / (sqrt(3)*Vs2); % Corrente nominal do inversor em RMS.
I_max_Bat1 = 1.5*Is_bat1*sqrt(2); % Corrente máxima do inversor em valor de pico (50% de sobrecarga).
    
% ----- Dados filtro CA ------ %   
L_Bat1 = 1.35E-3;    % Reat�ncia s�rie do filtro [H]
R_Bat1 = 0.1;        % Resistencia s�rie do filtro [ohms]
ron_Bat1 = 0.88E-3;  % Resistencia s�rie dos semicondutores [ohms]
Cf_Bat1 = 5E-6;      % Capacitancia shunt do filtro [F]

% ----- Dados do Elo CC (entre bateria e inversor) -----%
Vdc_Bat1 = 700;          % Tens�o nominal do elo CC
Vdc_Bat1 = 800;          % modificacao
Cdc_Bat1 = 2E-3; %19.24E-6/2;   % Capacitancia shunt do elo CC [F]

%% Dados de controle do inversor (VSI) 
% ----- Dados do controlador de corrente do VSI  ------ %  
ti_Bat1 = 0.5E-3; 
Kp_Bat1 = L_Bat1/ti_Bat1; 
Ki_Bat1 = (R_Bat1+ron_Bat1)/ti_Bat1; 
LPF_Ci_Bat1 = 8000; % Filtro PB do controlador (w em rad/s)
% bdw_ci = 2*pi() / (2*ti_Bat1); % largura de banda para consulta em rad/s

% ----- Dados do controlador de Tens�o do VSI  ------ %  
%%% cálculo %referencia Gkountaras(2017)%      
ang_m = 53 * pi()/180;
Ts = ti_Bat1/5; 
z_gk = (1 - sin(ang_m)) / (2*Ts*(1 + sin(ang_m))); 
w_c = sqrt(z_gk/Ts); 
k_gk = Cf_Bat1 * w_c; 

% Usando z do cálculo acima, k não funcionou, usar valor manual.
z_Bat1 = z_gk; 
Kv_Bat1 = 0.75;
LPF_Cv_Bat1 = 3000; % filtro sa�da dos controladores
K_iff_Bat1 = 0; %Ganho do laço feed forward de corrente 

% ----- Dados do controlador Potencia Droop do VSC  ------ %  
% Droop bat:
n_Bat1 = 0.05;
m_Bat1 = 0.05;
% Configura taxa da rampa de referência de potência P e Q
PQ_ref_taxavar_bat1 = 0.5; %0.5; %Taxa da rampa de ref de P e Q [p.u./s]
%Filtros: referencia Gkountaras(2017) e (Mohamed(2008))%
%...a designated range for this frequency isf LP F = 5(2) − 10Hz
LPF_P_Bat1 = 3*2*pi(); % 20 filtro sa�da do medidor de pot�ncia ativa
LPF_Q_Bat1 = 3*2*pi(); % filtro sa�da do medidor de pot�ncia reativa

% ----- Dados da Impedancia do Conector do VSC  ------ %  
%RC_Bat1=0.03;
%LC_Bat1=0.35e-3;

% ----- Dados da Impedancia virtual do VSC  ------ %  
R_Virt_Bat1 = 0; %0.002;
L_Virt_Bat1 = 3e-3; %0.15E-3;

%% Dados do Controle CC - Tensão elo CC e corrente Bateria %%

% ----- Dados do controlador de Corrente da Bateria  ------ %  
% Método: Delille (2010) - Contribution du Stockage à la Gestion Avancée des Systèmes Électriques
sig = 1; %amortecimento 
tr_ij = 10e-3; %tempo_resposta
Lfsj = 4e-3; %Indutância chopper (H) 
Rfsj = 0.03; %resistência da ind. chopper (ohm)
Tpj = 0.05e-3; %atraso de tempo da modulação
Tfsj = Lfsj/Rfsj;
w_ij = 5 / tr_ij;

% cálculo do controlador PI de corrente da bateria:
Kcij = Rfsj*(2*sig*w_ij*(Tpj + Tfsj)-1); 
Tcij = (2*sig*w_ij*(Tpj + Tfsj)-1) / (w_ij^2*(Tpj + Tfsj)); 
% BW_i = Kcij/Lfsj; apenas para ver largura de banda


% ----- Dados do controlador de Tensão Vdc do elo CC  ------ %  

% Método Pegueroles-Queralt (2014)
MDV = 0.9*Voc_bat1   ; %maximum discharge voltages (valor chute)
Vdc_ref = Vdc_Bat1;
Kvp_DC = 0.3;  
Kvi_DC = 25*Kvp_DC; 
% talvez esse filtro seja necessário no modelo chaveado
%LPF_Vdc = 500*2*pi(); 
% gráfico de ajuste do controlador, deixar comentado
%L_s = Kvp_DC/Cdc_Bat1 * tf([1 (Kvi_DC/Kvp_DC)] , [1 0 0]) * Vdc_ref/MDV;
%margin(L_s);



