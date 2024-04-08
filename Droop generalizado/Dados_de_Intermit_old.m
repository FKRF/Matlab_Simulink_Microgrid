%% Dados de entrada do inversor (retirados de Bidram_2013) Solar e E�lico: -- N�o Alterar
% neste script � apresentado os par�metros utilizados para as fontes
% intermitentes, com o controle dq0, assim como descritos em Yazdani 2010
% (livro VOLTAGE-SOURCED CONVERTERS IN POWER SYSTEMS)
%% Dados do filtro LRC
L = 1.35E-3;        % Reat�ncia s�rie do filtro [H]
R = 0.1;            % Resistencia s�rie do filtro [Ohms]
ron = 0.88E-3;      % Resistencia s�rie dos semicondutores [Ohms]
Cf = 5E-6;          % Capacit�ncia shunt do filtro [F]
%% Dados do sistema DC:
Vdc = 700;          % Tens�o no lado DC do conversor
Cdc = 19.24E-6/2;   % Capacit�ncia shunt do lado DC do conversor [F]
%% Dados do controlador de corrente:
ti = 0.5E-3;        % Constante de tempo para o controle de corrente (entre 0.5 a 5 ms) [s]
Kp = L/ti;          % Ganho proporcional [V/A]
Ki = (R+ron)/ti;    % Ganho integral [V/A]