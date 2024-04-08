%% Dados de controle Secundário
% Parâmetros do controle de tensão e frequência adotadas para a MR,
% retirados de Paulo_2020 (Dissertação)
%% Referencias de tensão, frequência e angulo (caso ativo)
Vref_sec = 1;           % referência de tensão [PU] (Vbase = 380)
wref_sec = 1;           % referência de frequência [PU] (Fbase = 60)
vref1=1;                % referência de tensão [PU] (Vbase = 22900)
cont_fre = 1;           % controle de frequência 1 - ativado; 0 - desativado;
cont_ten = 1;           % controle de tensão 1 - ativado; 0 - desativado;
%% controle de frequência
Kpw = 0.23;             % Ganho proporcional [p.u.]
Kiw = 2.5;              % Ganho integrativo [p.u.]
k_tracking = 50000;     % Ganho do sistema de tracking [p.u.]
TrM = 1;                % Variável para indicar se o sistema de tracking deve ser ativado ou não: 1 - ativado; 0 - desativado
%% controle de tensão
Kpe = 0.1;              % Ganho proporcional [p.u.]
Kie = 10;               % Ganho integrativo [p.u.]
% Obs.: não foi aplicado na dissertação...
%% Dados do controle de frequência descentralizado
a = 10;                 % Ganho proporcional [p.u.]
T2 = 5;                 % Constante de tempo do controle [s]
IHX = 1;                % Variável para indicar se o sistema de variável inicial deve ser ativado ou não: 1 - ativado; 0 - desativado
DSC = 1;                % Variável para indicar se o controle descentralizado deve ser ativado ou não: 1 - ativado; 0 - desativado
%% Dados para o alívio de carga ERAC
EAC = 0;                % Variável para indicar se o sistema de EAC deve ser ativado ou não: 1 - ativado; 0 - desativado
FAC1_1 = 58.50;         % Freq de corte para carga 1 de menor prioridade
FAC2_1 = 57.90;         % Freq de corte para carga 1 de prioridade média
FAC3_1 = 57.30;         % Freq de corte para carga 1 de maior prioridade
FAC1_2 = 58.50;         % Freq de corte para carga 2 de menor prioridade
FAC2_2 = 57.90;         % Freq de corte para carga 2 de prioridade média
FAC3_2 = 57.30;         % Freq de corte para carga 2 de maior prioridade
%% Dados de Atraso do controle secundário
ACM = 50E-5;            % Atraso de comunicação
TCD = 5E-3;             % Tempo do processamento discreto das funções de controle.
Td_sec = 1;             % Delay para entrada do controle centralizado