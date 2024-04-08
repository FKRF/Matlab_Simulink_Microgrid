%% Dados de controle Secund�rio
% Par�metros do controle de tens�o e frequ�ncia adotadas para a MR,
% retirados de Paulo_2020 (Disserta��o)
%% Referencias de tens�o, frequ�ncia e angulo (caso ativo)
Vref_sec = 1;           % refer�ncia de tens�o [PU] (Vbase = 380)
wref_sec = 1;           % refer�ncia de frequ�ncia [PU] (Fbase = 60)
vref1=1;                % refer�ncia de tens�o [PU] (Vbase = 22900)
cont_fre = 1;           % controle de frequ�ncia 1 - ativado; 0 - desativado;
cont_ten = 1;           % controle de tens�o 1 - ativado; 0 - desativado;
%% controle de frequ�ncia
Kpw = 0.23;             % Ganho proporcional [p.u.]
Kiw = 2.5;              % Ganho integrativo [p.u.]
k_tracking = 50000;     % Ganho do sistema de tracking [p.u.]
TrM = 1;                % Vari�vel para indicar se o sistema de tracking deve ser ativado ou n�o: 1 - ativado; 0 - desativado
%% controle de tens�o
Kpe = 0.1;              % Ganho proporcional [p.u.]
Kie = 10;               % Ganho integrativo [p.u.]
% Obs.: n�o foi aplicado na disserta��o...
%% Dados do controle de frequ�ncia descentralizado
a = 10;                 % Ganho proporcional [p.u.]
T2 = 5;                 % Constante de tempo do controle [s]
IHX = 1;                % Vari�vel para indicar se o sistema de vari�vel inicial deve ser ativado ou n�o: 1 - ativado; 0 - desativado
DSC = 1;                % Vari�vel para indicar se o controle descentralizado deve ser ativado ou n�o: 1 - ativado; 0 - desativado
%% Dados para o al�vio de carga ERAC
EAC = 0;                % Vari�vel para indicar se o sistema de EAC deve ser ativado ou n�o: 1 - ativado; 0 - desativado
FAC1_1 = 58.50;         % Freq de corte para carga 1 de menor prioridade
FAC2_1 = 57.90;         % Freq de corte para carga 1 de prioridade m�dia
FAC3_1 = 57.30;         % Freq de corte para carga 1 de maior prioridade
FAC1_2 = 58.50;         % Freq de corte para carga 2 de menor prioridade
FAC2_2 = 57.90;         % Freq de corte para carga 2 de prioridade m�dia
FAC3_2 = 57.30;         % Freq de corte para carga 2 de maior prioridade
%% Dados de Atraso do controle secund�rio
ACM = 50E-5;            % Atraso de comunica��o
TCD = 5E-3;             % Tempo do processamento discreto das fun��es de controle.
Td_sec = 1;             % Delay para entrada do controle centralizado