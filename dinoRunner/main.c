// Inclui o arquivo de cabeçalho da biblioteca Allegro 5
#include <allegro5/allegro.h>

//Fontes e arquivos de texto
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

// Inclui o cabeçalho do add-on para uso de imagens
#include <allegro5/allegro_image.h>

//Inclui os addo-ns de som
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include <allegro5/allegro_primitives.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LARGURA_TELA 800
#define ALTURA_TELA 600

//Trabalhando com sprites
#define FPS 60.0
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_BITMAP *folha_sprite = NULL;

// Variável representando a janela principal
ALLEGRO_DISPLAY *janela = NULL;

// Variável representando a imagem
ALLEGRO_BITMAP *imagem = NULL;

// O nosso arquivo de fonte
ALLEGRO_FONT *fonte = NULL;
ALLEGRO_FONT *fonteTitulo = NULL;

//Variáveis de reproducao de audio
ALLEGRO_SAMPLE *song = NULL;
ALLEGRO_SAMPLE_INSTANCE *songInstance = NULL;

ALLEGRO_SAMPLE *somPulo = NULL;
ALLEGRO_SAMPLE *gameOverSound = NULL;
ALLEGRO_SAMPLE *gameOverSoundChefao = NULL;


struct Background
{
    float x;
    float y;
    float velX;
    float velY;
    int dirX;
    int dirY;

    int width;
    int heigth;

    ALLEGRO_BITMAP *image;
};

struct Obstaculo
{
    float x;
    float y;
    float velX;
    float velY;
    int dirX;
    int dirY;

    int width;
    int heigth;

    ALLEGRO_BITMAP *image;
};

void initBackground(struct Background *back, float x, float y, float velX, float velY, int width, int heigth, int dirX, int dirY, ALLEGRO_BITMAP *image);
void updateBackground(struct Background *back);
void drawBackground(struct Background *back);

void initObstaculo(struct Obstaculo *obs, float x, float y,float velX, float velY, int width, int heigth, int dirX, int dirY, ALLEGRO_BITMAP *image);
void updateObstaculo(struct Obstaculo *obs);
void drawObstaculo(struct Obstaculo *obs);

int colidiu(float posx1, float posy1, float posx2, float posy2, float alturaObj1, float larguraObj1, float alturaObj2, float larguraObj2);

//Variavel referenciando o plano de fundo
ALLEGRO_BITMAP *bgImage = NULL;
ALLEGRO_BITMAP *bgImage2 = NULL;
ALLEGRO_BITMAP *bgImage3 = NULL;
ALLEGRO_BITMAP *bgImage4 = NULL;
ALLEGRO_BITMAP *bgImage5 = NULL;
ALLEGRO_BITMAP *bgImage6 = NULL;

//Imagens da tela de fundo
ALLEGRO_BITMAP *bgMenuImagem = NULL;
ALLEGRO_BITMAP *imgDino1 = NULL;
ALLEGRO_BITMAP *imgDino2 = NULL;


ALLEGRO_BITMAP *obsImage1 = NULL;
ALLEGRO_BITMAP *obsImage2 = NULL;
ALLEGRO_BITMAP *obsImage3 = NULL;

ALLEGRO_BITMAP *lifeHeart = NULL;//Coração que indica quantas vidas vc já teve



struct Background *BG1;
struct Background *BG2;
struct Background *BG3;
struct Background *BG4;
struct Background *BG5;
struct Background *BG6;


struct Obstaculo *OBS1;
struct Obstaculo *OBS2;
struct Obstaculo *OBS3;


struct Obstaculo *obstaculos[3];




//função que mostra mensagem de erro em uma caixa de texto nativa
//a função recebe a string da mensagem como parâmetro
void error_msg(char *text)
{
    al_show_native_message_box(NULL,"ERRO",
                               "Ocorreu o seguinte erro e o programa sera finalizado:",
                               text,NULL,ALLEGRO_MESSAGEBOX_ERROR);
}

int createBackgroundMusic()
{
    //Definindo quantos arquivos de som estão sendo tocados ao mesmo tempo
    al_reserve_samples(1);

    //Definição do arquivo de som
    song = al_load_sample("sound/somFundo.wav");
    if(!song)
    {
        error_msg("Arquivo de som invalido!");
        return -1;
    }

    songInstance = al_create_sample_instance(song);
    al_set_sample_instance_gain(songInstance, 0.5);
    //Faz com que o som de fundo toque em loop
    al_set_sample_instance_playmode(songInstance, ALLEGRO_PLAYMODE_LOOP);

    //Linkar o arquivo de som ao mixer de som
    al_attach_sample_instance_to_mixer(songInstance, al_get_default_mixer());
    return 1;
}

int inicializar()
{
    //tenta inciar a biblioteca, caso retorne false, mostra mensagem e encerra o programa
    if (!al_init())
    {
        error_msg("Falha ao inicializar a Allegro");
        return -1;
    }

    // Inicializa o add-on para utilização de imagens
    al_init_image_addon();

    // Inicialização do add-on para uso de fontes
    al_init_font_addon();

    // Inicialização do add-on para uso de fontes True Type
    if (!al_init_ttf_addon())
    {
        error_msg("Falha ao inicializar add-on allegro_ttf");
        return -1;
    }

    timer = al_create_timer(1.0 / FPS);
    if(!timer)
    {
        error_msg("Falha ao criar temporizador");
        return -1;
    }

    //inicializa addon do teclado
    if (!al_install_keyboard())
    {
        error_msg("Falha ao inicializar o teclado");
        return 0;
    }

    // Criamos a janela
    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!janela)
    {
        error_msg("Falha ao criar janela");
        al_destroy_timer(timer);
        return -1;
    }
    al_set_window_title(janela, "RUN DINO, RUN!");

    fila_eventos = al_create_event_queue();
    if(!fila_eventos)
    {
        error_msg("Falha ao criar fila de eventos");
        al_destroy_timer(timer);
        al_destroy_display(janela);
        return 0;
    }

    // Carregando o arquivo de fonte
    fonte = al_load_font("ARCADECLASSIC.ttf", 38, 0);
    fonteTitulo = al_load_font("ARCADECLASSIC.TTF", 58,0);
    if (!fonte || !fonteTitulo)
    {
        al_destroy_display(janela);
        error_msg("Falha ao carregar fonte");
        return -1;
    }

    folha_sprite = al_load_bitmap("img/dinos/doux.bmp");
    if (!folha_sprite)
    {
        error_msg("Falha ao carregar sprites");
        al_destroy_timer(timer);
        al_destroy_display(janela);
        al_destroy_event_queue(fila_eventos);
        return -1;
    }


    //usa a cor verde marca-texto como transparencia
    al_convert_mask_to_alpha(folha_sprite,al_map_rgb(0,255,0));

//--------------------------------------------------------------------------------------------------------
//CONFIGURANDO IMAGEM DE FUNDO PRINCIPAL
    // Carrega a imagem
    bgImage = al_load_bitmap("img/background/layer1.bmp");
    bgImage2 = al_load_bitmap("img/background/Hills Layer 02.png");
    bgImage3 = al_load_bitmap("img/background/Hills Layer 03.png");
    bgImage4 = al_load_bitmap("img/background/Hills Layer 04.png");
    bgImage5 = al_load_bitmap("img/background/layer5.bmp");
    bgImage6 = al_load_bitmap("img/background/nuvens.png");

    bgMenuImagem = al_load_bitmap("img/background/fundoMenu.jpg");

    obsImage1 = al_load_bitmap("img/cactus1.png");
    obsImage2 = al_load_bitmap("img/cactus2.png");
    obsImage3 = al_load_bitmap("img/rock.png");

    imgDino1 = al_load_bitmap("img/dino1.png");
    imgDino2 = al_load_bitmap("img/dino2.png");



    initBackground(BG1, 0,0,2,0,LARGURA_TELA, ALTURA_TELA,-1,1,bgImage);
    initBackground(BG2, 0,0,2,0,LARGURA_TELA, 150,-1,1,bgImage2);
    initBackground(BG3, 0,0,3,0,LARGURA_TELA, ALTURA_TELA,-1,1,bgImage3);
    initBackground(BG4, 0,0,4,0,LARGURA_TELA, 150,-1,1,bgImage4);
    initBackground(BG5, 0,0,9,0,LARGURA_TELA, 150,-1,1,bgImage5);
    initBackground(BG6, 0,0,2,0,LARGURA_TELA, ALTURA_TELA,-1,1,bgImage6);




//--------------------------------------------------------------------------------------------------------
//CONFIGURAÇÕES DE MUSICA DE FUNDO
    al_install_audio();
    al_init_acodec_addon();
    //Carrega as configuracoes do som de fundo
    if(!createBackgroundMusic())
    {
        return 0;
    }
    al_play_sample_instance(songInstance);
//--------------------------------------------------------------------------------------------------------
//CONFIGURAR A MÚSICA DE PULO
    somPulo = al_load_sample("sound/pulo.wav");
    gameOverSound = al_load_sample("sound/gameOver.ogg");
    gameOverSoundChefao = al_load_sample("sound/gameOverBad.ogg");

    al_reserve_samples(3); //O número de samples de som que está usando
//--------------------------------------------------------------------------------------------------------
//CONFIGURAR A MÚSICA DE GAME OVER
//--------------------------------------------------------------------------------------------------------
    //registra tres fontes de eventos na fila. o da janela, e do teclado e do timer

    al_register_event_source(fila_eventos, al_get_display_event_source(janela));
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());

    al_start_timer(timer);


    // Desenha a imagem na tela
    //Dois valores representando as coordenadas X e Y da tela, comecando do canto superior esquerdo
    al_draw_bitmap(bgImage, 0, 0, 0);

    //Desenha o texto centralizado
    al_draw_text(fonteTitulo, al_map_rgb(0, 0, 0), LARGURA_TELA / 2, 90, ALLEGRO_ALIGN_CENTRE, "DINO RUNNER");

    // Atualiza a tela
    al_flip_display();

    return 1;
}

int estaNoChao(int posAtualY, int pixelsChao)
{
    if(posAtualY>pixelsChao)
        return 0; //Não está no chão
    else
    {
        return 1;//Está no chão
    }
}

struct Obstaculo* selectObstaculo()
{
    int r = 0;
    r = rand()%3;
    return obstaculos[r];
}

long lerMaiorPontuacao(FILE *arquivoMaiorPontuacao)
{
    char line[80];
    char *ptr;
    int count = 0;
    char ch = 'a';
    line[0] = EOF;
    while(ch!=EOF)
    {
        ch=fgetc(arquivoMaiorPontuacao);
        if(ch!=EOF)
        {
            line[count] = ch;
            count++;
        }
    }
    if(line[0]!=EOF)
    {
        return strtol(line, &ptr, 10);
    }
    return 0;
}

void limparLixo()
{
//Limpando o que deixou pra trás pra finalizar o programa
    al_destroy_bitmap(folha_sprite);
    al_destroy_bitmap(imagem);
    al_destroy_timer(timer);
    al_destroy_display(janela);
    al_destroy_event_queue(fila_eventos);
    al_destroy_sample(song);
    al_destroy_sample(somPulo);
    al_destroy_sample(gameOverSound);


    al_destroy_sample_instance(songInstance);

    al_destroy_bitmap(bgImage);
    al_destroy_bitmap(bgImage2);
    al_destroy_bitmap(bgImage3);
    al_destroy_bitmap(bgImage4);
    al_destroy_bitmap(bgImage5);
    al_destroy_bitmap(obsImage1);
    al_destroy_bitmap(obsImage2);
    al_destroy_bitmap(obsImage3);
    al_destroy_bitmap(imgDino1);
    al_destroy_bitmap(imgDino2);
    al_destroy_bitmap(bgMenuImagem);
}


int jogar(int iniciar)
{
    int enter = iniciar;
    FILE *arquivoMaiorPontuacao = NULL;
    arquivoMaiorPontuacao = fopen("pontos.txt", "r");
    long maiorPontuacao = lerMaiorPontuacao(arquivoMaiorPontuacao);
    fclose(arquivoMaiorPontuacao);
    float numRand;
    int contaFrames = 0;
    long tempoDeJogo = 0;

    int bateu = 0;

    int desenha = 1;
    int sair = 0;
    int tecla = 0;//1 marca apertar a tecla espaço; 2 marca soltar espaço
    float minimoY;
    float pixelsChaoY = 500;
    float tamanhoPulo = 250;
    minimoY = pixelsChaoY-tamanhoPulo;

    BG1 = (struct Background*)malloc(sizeof(struct Background));
    BG2 = (struct Background*)malloc(sizeof(struct Background));
    BG3 = (struct Background*)malloc(sizeof(struct Background));
    BG4 = (struct Background*)malloc(sizeof(struct Background));
    BG5 = (struct Background*)malloc(sizeof(struct Background));
    BG6 = (struct Background*)malloc(sizeof(struct Background));

    OBS1 = (struct Obstaculo*)malloc(sizeof(struct Obstaculo));
    OBS2 = (struct Obstaculo*)malloc(sizeof(struct Obstaculo));
    OBS3 = (struct Obstaculo*)malloc(sizeof(struct Obstaculo));

    //largura e altura de cada sprite dentro da folha
    float altura_sprite=24, largura_sprite=24;
    //quantos sprites tem em cada linha da folha, e a atualmente mostrada
    float colunas_folha=15, coluna_atual=0;
    //quantos sprites tem em cada coluna da folha, e a atualmente mostrada
    float linha_atual=0, linhas_folha=1;
    //posicoes X e Y da folha de sprites que serao mostradas na tela
    float regiao_x_folha=100, regiao_y_folha=0;
    //quantos frames devem se passar para atualizar para o proximo sprite
    float frames_sprite=6, cont_frames=0;
    //posicao X Y da janela em que sera mostrado o sprite
    float pos_x_sprite=100, pos_y_sprite=pixelsChaoY;
    //velocidade X Y que o sprite ira se mover pela janela
    float vel_x_sprite=0, vel_y_sprite=0;


    //Caso tenha dado algum erro
    if(!inicializar())
    {
        return -1;
    }

    obstaculos[0] = OBS1;
    obstaculos[1] = OBS2;
    obstaculos[2] = OBS3;

    initObstaculo(OBS1,LARGURA_TELA+rand()%5*100, pixelsChaoY-al_get_bitmap_height(obsImage1)/3, -4.5,0,75,100,-2,0,obsImage1);
    initObstaculo(OBS2,LARGURA_TELA+rand()%5*100, pixelsChaoY-al_get_bitmap_height(obsImage2)/3, -4.5,0,88,100,-2,0,obsImage2);
    initObstaculo(OBS3,LARGURA_TELA+rand()%5*100, pixelsChaoY-al_get_bitmap_height(obsImage3)/3, -4.5,0,73,120,-2,0,obsImage3);

    struct Obstaculo *selecionado = selectObstaculo();
    struct Obstaculo *selecionado2;

    const float gravidade = 9.8;
    float timeCount = 0.0;
    int pulando = 0;
    int podePular = 1;
    al_draw_bitmap(bgMenuImagem,0, 0, 0);
    int count = 0;
    while(enter==0)
    {
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);
        if(count > 20)
        {
            count = 0;
            al_draw_text(fonte, al_map_rgb(255, 0, 0), LARGURA_TELA/2, ALTURA_TELA/2, ALLEGRO_ALIGN_CENTRE, "PRESS ENTER TO START");
            al_draw_text(fonteTitulo, al_map_rgb(255, 0, 0), LARGURA_TELA/2, ALTURA_TELA/2-100, ALLEGRO_ALIGN_CENTRE, "DINO RUNNER");
        }
        else if(count == 10)
        {
            al_draw_text(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA/2, ALTURA_TELA/2, ALLEGRO_ALIGN_CENTRE, "PRESS ENTER TO START");
            al_draw_text(fonteTitulo, al_map_rgb(0, 0, 0), LARGURA_TELA/2, ALTURA_TELA/2-100, ALLEGRO_ALIGN_CENTRE, "DINO RUNNER");
        }
        al_draw_text(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA/2, 100, ALLEGRO_ALIGN_CENTRE, "PRESS DEL TO CLEAR SCORE");

        al_draw_bitmap(imgDino1, 0, 400, 0);
        al_draw_bitmap(imgDino2, 600, 400, 0);
        al_flip_display();
        if(evento.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            switch (evento.keyboard.keycode)
            {
            case ALLEGRO_KEY_ENTER:
                enter = 1;
                break;
            case ALLEGRO_KEY_ESCAPE:
                enter = 1;
                sair = 1;
                break;

            case ALLEGRO_KEY_DELETE:
                //Pra pessoa zerar a pontuação
                arquivoMaiorPontuacao = fopen("pontos.txt", "w");
                fclose(arquivoMaiorPontuacao);
                maiorPontuacao = 0;
                enter = 1;
                break;

            }
        }
        else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            sair = 1;
            enter = 1;
        }
        count+=1;
    }

    while(!sair)
    {
        if(contaFrames == FPS)
        {
            contaFrames = 0;
            tempoDeJogo+=1;
        }
        else
        {
            contaFrames++;
        }
        numRand = rand()%100;
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);
        if(numRand<90 && selecionado->x==-100)
        {
            selecionado = selectObstaculo();
        }
        /* -- EVENTOS -- */
        if(evento.type == ALLEGRO_EVENT_TIMER)
        {
            //a cada disparo do timer, incrementa cont_frames
            cont_frames++;
            //se alcancou a quantidade de frames que precisa passar para mudar para o proximo sprite
            if (cont_frames >= frames_sprite)
            {
                //reseta cont_frames
                cont_frames=0;
                //incrementa a coluna atual, para mostrar o proximo sprite
                coluna_atual++;

                //se coluna atual passou da ultima coluna
                if (coluna_atual >= colunas_folha)
                {
                    //volta pra coluna inicial
                    coluna_atual=0;
                    //incrementa a linha, se passar da ultima, volta pra primeira
                    linha_atual = fmod((linha_atual+1), linhas_folha);
                    //calcula a posicao Y da folha que sera mostrada
                    regiao_y_folha = linha_atual * altura_sprite;
                }
                //calcula a regiao X da folha que sera mostrada
                regiao_x_folha = coluna_atual * largura_sprite;

            }

            //se o sprite estiver perto da borda direita ou esquerda da tela
//            if ( pos_x_sprite + largura_sprite > LARGURA_TELA - 20 || pos_x_sprite < 20 )
//            {
//                //inverte o sentido da velocidade X, para andar no outro sentido
//                vel_x_sprite = -vel_x_sprite;
//            }

            updateBackground(BG1);
            updateBackground(BG2);
            BG2->y = 150;
            updateBackground(BG3);
            BG3->y = 150;
            updateBackground(BG4);
            BG4->y = 150;
            updateBackground(BG5);
            updateBackground(BG6);
            if(tempoDeJogo == 10)
            {
                BG5->velX = 10;
                selecionado->velX = -5;
            }
            else if(tempoDeJogo == 15)
            {
                BG5->velX = 12;
                selecionado->velX = -6;

            }
            else if(tempoDeJogo == 20)
            {
                BG5->velX = 14;
                selecionado->velX = -7;

            }
            else if(tempoDeJogo == 50)
            {
                BG5->velX = 16;
                selecionado->velX = -9;
            }
            desenha=1;

        }
        else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            sair = 1;

        }
        else if(evento.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            if(podePular)
            {
                switch(evento.keyboard.keycode)
                {
                case ALLEGRO_KEY_SPACE:
                    podePular = 0;
                    tecla = 1;
                    pulando = 1;
                    //Apertou a tecla para começar o pulo
                    timeCount+=0.1;
                    al_play_sample(somPulo, 2.0, 0.0, 1.5, ALLEGRO_PLAYMODE_ONCE, 0);
                    pos_y_sprite-=timeCount*gravidade;
                    break;

                }
            }
        }
        else if(evento.type == ALLEGRO_EVENT_KEY_UP)
        {

            switch(evento.keyboard.keycode)
            {
            case ALLEGRO_KEY_SPACE:
                tecla = 2;
                pulando = 0;
                timeCount+=0.1;
                vel_y_sprite = 0;//Ponto mais alto da subida
                break;
            }
        }

//        //atualiza as posicoes X Y do sprite de acordo com a velocidade, positiva ou negativa
//            pos_x_sprite += vel_x_sprite;
//            pos_y_sprite += vel_y_sprite;


        /* -- ATUALIZA TELA -- */
        if(desenha && al_is_event_queue_empty(fila_eventos))
        {
            float tempoAlto = 0.0;
            while(pos_y_sprite==minimoY)
            {
                tempoAlto+=0.1;
                if(tempoAlto>=2)
                {
                    pulando = 0;
                    break;
                }
            }
            //al_rest(1.0);
            if(pos_y_sprite<pixelsChaoY && pulando)
            {
                timeCount+=0.1;
                pos_y_sprite-=timeCount*gravidade;
                if(pos_y_sprite<=minimoY) //Definição de um tamanho máximo pra pular
                {
                    pos_y_sprite = minimoY;
                }
            }
            if(pos_y_sprite<pixelsChaoY && !pulando)
            {
                if(pos_y_sprite<=minimoY)
                    timeCount = 0;
                timeCount+=0.1;
                pos_y_sprite += timeCount*gravidade;
                if(pos_y_sprite>pixelsChaoY)//Não pode passar do chão
                {
                    podePular = 1;
                    pos_y_sprite = pixelsChaoY;
                    timeCount = 0;
                }
            }
            //desenha o fundo na tela
            drawBackground(BG1);
            drawBackground(BG2);
            drawBackground(BG3);
            drawBackground(BG4);
            drawBackground(BG5);
            drawBackground(BG6);
            drawObstaculo(selecionado);

            //al_draw_bitmap_region(imagem,0,0,LARGURA_TELA,ALTURA_TELA,0,0,0);
            al_draw_text(fonteTitulo, al_map_rgb(0, 0, 0), LARGURA_TELA / 2, 90, ALLEGRO_ALIGN_CENTRE, "DINO RUNNER");
            al_draw_textf(fonte, al_map_rgb(0, 0, 0), LARGURA_TELA -200, 30, ALLEGRO_ALIGN_CENTRE, "SCORE %i",tempoDeJogo);
            if(tempoDeJogo < maiorPontuacao)
            {
                al_draw_textf(fonte, al_map_rgb(0, 0, 0), 250, 30, ALLEGRO_ALIGN_CENTRE, "MAX SCORE  %ld",maiorPontuacao);
            }
            else
            {
                al_draw_textf(fonte, al_map_rgb(0, 0, 0), 250, 30, ALLEGRO_ALIGN_CENTRE, "MAX SCORE  %i",tempoDeJogo);
            }
            /*--------DESENHANDO OBSTACULOS----------*/


            //velocidade positiva (movendo para direita)
            if (vel_x_sprite > 0)
                //desenha sprite na posicao X Y da janela, a partir da regiao X Y da folha
//                al_draw_bitmap_region(folha_sprite,
//                                      regiao_x_folha,regiao_y_folha,
//                                      largura_sprite,altura_sprite,
//                                      pos_x_sprite,pos_y_sprite,0);

                al_draw_scaled_bitmap(folha_sprite,
                                      regiao_x_folha,regiao_y_folha,
                                      largura_sprite,altura_sprite,
                                      pos_x_sprite+largura_sprite,pos_y_sprite,
                                      -largura_sprite*3,altura_sprite*3,0);
            else
                //desenha sprite, igual acima, com a excecao que desenha a largura negativa, ou seja, espelhado horizontalmente
                al_draw_scaled_bitmap(folha_sprite,
                                      regiao_x_folha,regiao_y_folha,
                                      largura_sprite,altura_sprite,
                                      pos_x_sprite+largura_sprite,pos_y_sprite,
                                      largura_sprite*3,altura_sprite*3,0);



            if(colidiu(pos_x_sprite,pos_y_sprite,selecionado->x, selecionado->y, altura_sprite, largura_sprite, selecionado->heigth,selecionado->width) == 1)
            {
                bateu = 1;
                sair = 1;
                al_play_sample(gameOverSound, 2.0, 0.0, 1.5, ALLEGRO_PLAYMODE_ONCE, 0);
                al_play_sample(gameOverSoundChefao, 2.0, 0.5, 1.5, ALLEGRO_PLAYMODE_ONCE, 0);
                enter = 0;
                while(enter == 0)
                {
                    al_wait_for_event(fila_eventos, &evento);

                    al_draw_text(fonteTitulo, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, (ALTURA_TELA/2)-100, ALLEGRO_ALIGN_CENTRE, "GAME OVER");
                    al_draw_textf(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, (ALTURA_TELA/2), ALLEGRO_ALIGN_CENTRE, "FINAL SCORE %d", tempoDeJogo);
                    al_draw_text(fonte, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, (ALTURA_TELA/2)+100, ALLEGRO_ALIGN_CENTRE, "PRESS ENTER TO START AGAIN");

                    al_flip_display();
                    if(evento.type == ALLEGRO_EVENT_KEY_DOWN)
                    {
                        switch (evento.keyboard.keycode)
                        {
                        case ALLEGRO_KEY_ENTER:
                            enter = 1;
                            break;
                        }
                    }
                    else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                    {
                        enter = 1;
                        sair = 1;
                        break;
                    }
                }
            }

            al_flip_display();
            desenha = 0;
            updateObstaculo(selecionado);
            if(selecionado->x==-100)
            {
                selecionado = selectObstaculo();
            }
        }
        tecla = 0;
    }

    if(tempoDeJogo>maiorPontuacao)
    {
        //O jogador fez mais pontos do que a maior pontuação
        char line[80];
        arquivoMaiorPontuacao = fopen("pontos.txt", "w");
        sprintf(line, "%lu", tempoDeJogo);
        fputs(line,arquivoMaiorPontuacao);
        fclose(arquivoMaiorPontuacao);
    }

    limparLixo();

    free(BG1);
    free(BG2);
    free(BG3);
    free(BG4);
    free(BG5);
    free(OBS1);
    free(OBS2);
    free(OBS3);

    if(bateu == 1)
        return 1;
    return 0;
}


void initBackground(struct Background *back, float x, float y, float velX, float velY, int width, int heigth, int dirX, int dirY, ALLEGRO_BITMAP *image)
{
    back->x = x;
    back->y = y;
    back->velX = velX;
    back->velY = velY;
    back->width = width;
    back->heigth = heigth;
    back->dirX = dirX;
    back->dirY = dirY;
    back->image = image;
}

void initObstaculo(struct Obstaculo *obs, float x, float y,float velX, float velY, int width, int heigth, int dirX, int dirY, ALLEGRO_BITMAP *image)
{
    obs->x = x;
    obs->y = y;
    obs->velX = velX;
    obs->velY = velY;
    obs->width = width;
    obs->heigth = heigth;
    obs->dirX = dirX;
    obs->dirY = dirY;
    obs->image = image;
}

void updateObstaculo(struct Obstaculo *obs)
{
    int r = rand()%3;
    if((obs->x) - (obs->velX * obs->dirX) >=-100)
    {
        obs->x =(obs->x) - (obs->velX * obs->dirX);
    }
    else
    {
        obs->x = LARGURA_TELA+(r*2);
    }
}

void updateBackground(struct Background *back)
{
    back->x += back->velX * back->dirX;
    if((back->x)+ (back->width) <= 0 )
    {
        //Isto irá fazer com que o plano de fundo reinicie quando o rolamento chegar no final da imagem de fundo
        back->x = 0;
    }

}

void drawObstaculo(struct Obstaculo *obs)
{
    al_draw_bitmap(obs->image, obs->x,obs->y, 0);
}

void drawBackground(struct Background *back)
{
    al_draw_bitmap(back->image, back->x,back->y, 0);
    if(back->x + back->width < LARGURA_TELA)
        al_draw_bitmap(back->image, back->x + back->width, back->y, 0);
}

int colidiu(float posx1, float posy1, float posx2, float posy2, float alturaObj1, float larguraObj1, float alturaObj2, float larguraObj2)
{
    //Analisa as posicoes do jogador e do obstaculo
    //Retorna 0 se nao colidiu
    //Retorna 1 se colidiu
    if( (posx1-(larguraObj1/2)) > ( posx2+(larguraObj2/2)) )
    {
        //O obstaculo já passou do dino, então não foi colisão
        return 0;
    }

    if( (posx1+(larguraObj1/2)) >= (posx2 -(larguraObj2/2)) )  //tratando na horizontal pela frente do objeto
    {
        //Bateu de frente

        //Ver se o dino não está pulando
        if( (posy1+(alturaObj1/2)) >= (posy2-(alturaObj2/2)) )  //o pulo nao foi suficiente
        {
            return 1;
        }
    }

    return 0;
}

int main(void)
{
    while(jogar(0) == 1);

    return 0;
}


