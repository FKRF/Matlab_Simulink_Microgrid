%% Rodar Simulink e salvar variáveis 
% Procedimentos para rodar vários casos automaticamente:
% 1) definir a pasta (diretório) em que será salvo os arquivos (a variável
% PASTA neste script tem como objetivo definir o diretório) 
% 2) Definir os N eventos que se deseja realizar, e criar variáveis com
% esses eventos (EX: 6 eventos com a variável "HorarioV = [1 2 3 4 5 6];");
% 3) Definir as condições da MR, tipo de controles, cargas, etc...
% 4) Criar um laço de repetição (For ou while) o qual deverá executar o
% laço N vezes;
% 5) Dentro do laço atribuir as variáveis de simulação com o respectivo
% valor da variável de evento criadas no passo 2 (EX: Horario =
% HorarioV(i) - i é a variável que indica o numero do evento);
% 6) Em sequência definir uma variável que gera um novo diretório para cada
% simulação (EX: Dir = [PASTA NAME], em que NAME deve ser uma variável que
% mude de nome a cada interação, para que cada diretório seja diferente);
% 7) Em sequência incluir o comando "mkdir(DIR);" para gerar o diretório
% automaticamente;
% 8) Em sequência incluir o comando "sim('MR_CIGRE_BASE_V1.slx');", assim o
% simulink da MR será executado automaticamente;
% 9) Em sequência usar incluir os comandos para salvar as variáveis da MR
% "save([DIR 'MGCI.mat'],'MGCC_VI')";
% 10) Finalizar o laço de repetição.
% 
% Obs.: É possível gerar gráficos pós uma simulação ou pós todas as
% simulações.
%% SIMULAR EVENTOS
%% 1) definir a pasta (diretório)
PASTA = ['Artigo_1\']; % Definir o diretório das simulações 
% Obs.: Deve ser colocado "\" no fim do nome do diretório, do contrário não
% funcionara a lógica proposta
 
%% 2) Definir os N eventos que se deseja realizar
N = 6;                              % Número de eventos 
HorarioV = [1 2 3 1 2 3];           % Vetor para variável do horário
Pref_SolarV = [0 0 0 20 20 20]*1e3; % Vetor para variável de potência ativa do painel solar (PV)
 
%% 3) Definir as condições da MR
Qref_Solar = 0;                     % Potência reativa do painel solar
t_sim = 4;                          % tempo de ilhamento
% Obs.: Se não for definidas as variáveis nesta etapa, serão selecionadas  
% as condições apresentadas nos demais scripts serão selecionadas. No exemplo
% mostrado a potência reativa do PV será mantida em 0, e o tempo de
% ilhamento ocorrera em 4 segundos de simulação, independentes dos valores
% apresentados no script Dados_MR_CIGRE_Modelo. 
 
%% 4) Criar um laço de repetição
for i=1:N
    %% 5) Atribuir as variáveis de simulação
    Horario = HorarioV(i);          % Horário de carga
    Pref_Solar = Pref_SolarV(i);    % Potência ativa gerada pelo PV
    % Obs.: a cada laço de repetição as variáveis iram ganhar novos valores
    
    %% 6) Definir uma variável que gera um novo diretório
    NAME = ['H_' num2str(Horario) 'PV_P_' num2str(Pref_Solar) '\'];     % nome do novo diretório
    Dir = [PASTA NAME];                                                 % Variável de diretório
    % Obs.: é possível usar uma variável de vetor com caracteres para gerar o
    % nome.
    %Obs. 2: Usar \ no fim da variável NAME... 
    
    %% 7) Gerar o diretório automaticamente
    mkdir(DIR);                     % Função para gerar o diretório
    
    %% 8) Executar simulink automaticamente
    sim('MR_CIGRE_BASE_V1.slx');
    % Obs.: Caso queira executar outro modelo da MR altere o nome do
    % arquivo para o desejado
    
    %% 9) Salvar as variáveis da MR    
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
    % Obs.: No comando save, a primeira entrada indica o nome da variável,
    % e o segundo o nome da variável no Simulink.
    
    %% 10) Finalizar o laço de repetição
end
