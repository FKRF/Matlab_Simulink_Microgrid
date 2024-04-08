%% Dados de entrada do inversor (retirados de Bidram_2013) Solar e Eolico: -- Nï¿½o Alterar
L = 1.35E-3;    % Reatï¿½ncia sï¿½rie do filtro [H]
R = 0.1;        % Resistencia sï¿½rie do filtro [ohms]
ron = 0.88E-3;  % Resistencia sï¿½rie dos semicondutores [ohms]
Cf = 5E-6;      % Capacitancia shunt do filtro [F]
% dados do sistema DC:
Vdc = 700;          % Tensï¿½o no lado DC do conversor
Cdc = 19.24E-6/2;   % Capacitancia shunt do lado DC do conversor [F]
% dados do controlador de corrente:
ti = 0.5E-3;
Kp_interm = L/ti;
Ki_interm = (R+ron)/ti;

%     % Modelo: Barbado, Gustavo (2019) - DissertaÃ§Ã£o
%     % dados do controlador de potÃªncia:
%     Kp_cp = 0.2; %0.2
%     ti_cp = 0.5e-3; % 0.5e-3 1e-3 
%     % dados do controlador de potÃªnci reativa:
%     Kp_cr = 0.4e-3;
%     ti_cr = 5e-3; %10e-3; 
%     LPF_P_Interm = 100; 
    
    % Modelo: Barbado, Gustavo (2019) - Dissertação
    % dados do controlador de potência:
    Kp_cp = 0.2; %0.2
    ti_cp = 10e-3; % 0.5e-3 1e-3 
    % dados do controlador de potênci reativa:
    Kp_cr = 0.4e-3;
    ti_cr = 10e-3; %5e-3 10e-3; 
    LPF_P_Interm = 100;

    % Dados para limitador de corrente do inversor
    %Corrente nominal RMS do inversor solar
    I_nom_solar1 = Sb_sol1/(Vs2*sqrt(3));
    I_nom_solar2 = Sb_sol2/(Vs2*sqrt(3));

    %MÃ¡xima corrente do inversor solar em valor de pico (em sobrecarga)
    Imax_solar1 = 1.5 * I_nom_solar1 * sqrt(2); % 1.5 Vezes a corrente nominal 
    Imax_solar2 = 1.5 * I_nom_solar2 * sqrt(2); % 1.5 Vezes a corrente nominal 


