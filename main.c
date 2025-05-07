// Versão finalizada do projeto
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <GL/glut.h>
#include <GL/gl.h>

float corSelec[4];
int tipoSelec = 0;
int selecionandoCor = 0;
int modoSelecao = 0;

struct ponto
{
  float mat[3][1];
  float cor[4]; // RGBA
  struct ponto *prox;
} typedef ponto;

struct forma
{
  int tipo;
  int numPts;
  int finalizado;
  struct forma *ant;
  struct forma *prox;
  struct ponto *pts;
  struct ponto *ultimoPt;
} typedef forma;

int pularIntro = 0;

forma *primeiro = NULL;
forma *ultimo = NULL;
forma *formaSelecionada = NULL;

void multiplicaMatriz(float mat1[3][3], float mat2[3][1], float res[3][1])
{
  for (int i = 0; i < 3; i++)
  {
    res[i][0] = 0.0f;
    for (int j = 0; j < 3; j++)
    {
      res[i][0] += mat1[i][j] * mat2[j][0];
    }
  }
}

void criarNovaForma()
{
  if (ultimo != NULL && ultimo->pts == NULL)
  {
    ultimo->tipo = tipoSelec;
    return;
  }

  forma *novaForma = (forma *)malloc(sizeof(forma));
  if (novaForma == NULL)
  {
    printf("%d", "Erro ao criar nova forma\n");
  }

  novaForma->numPts = 0;
  novaForma->finalizado = 0;
  novaForma->tipo = tipoSelec;
  novaForma->pts = NULL;
  novaForma->prox = NULL;
  novaForma->ant = ultimo;

  if (ultimo != NULL)
  {
    ultimo->prox = novaForma;
  }
  ultimo = novaForma;

  if (primeiro == NULL)
  {
    primeiro = novaForma;
  }
}

void deletarForma(forma *formaADeletar)
{

  if (formaADeletar == NULL)
  {
    return;
  }

  if (formaADeletar == primeiro)
  {
    primeiro = formaADeletar->prox;
    if (primeiro != NULL)
    {
      primeiro->ant = NULL;
    }
  }

  if (formaADeletar == ultimo)
  {
    ultimo = formaADeletar->ant;
    if (ultimo != NULL)
    {
      ultimo->prox = NULL;
    }
  }

  if (formaADeletar->ant != NULL)
  {
    formaADeletar->ant->prox = formaADeletar->prox;
  }
  if (formaADeletar->prox != NULL)
  {
    formaADeletar->prox->ant = formaADeletar->ant;
  }

  ponto *pontoAtual = formaADeletar->pts;
  while (pontoAtual != NULL)
  {
    ponto *proxPt = pontoAtual->prox;
    free(pontoAtual);
    pontoAtual = proxPt;
  }
  if (formaSelecionada == formaADeletar)
  {
    formaSelecionada = NULL;
  }
  free(formaADeletar);
}

void salvar()
{
  FILE *file = fopen("imagem.pnt", "w");
  if (file == NULL)
  {
    printf("Erro ao salvar\n");
    return;
  }

  forma *formaAtual = primeiro;
  while (formaAtual != NULL)
  {
    fprintf(file, "%d %d\n", formaAtual->tipo, formaAtual->numPts);
    ponto *pontoAtual = formaAtual->pts;
    while (pontoAtual != NULL)
    {
      fprintf(file, "%f %f %f %f %f\n", pontoAtual->mat[0][0], pontoAtual->mat[1][0], pontoAtual->cor[0], pontoAtual->cor[1], pontoAtual->cor[2]);
      pontoAtual = pontoAtual->prox;
    }
    formaAtual = formaAtual->prox;
  }

  fclose(file);
}
void carregar()
{
  FILE *file = fopen("imagem.pnt", "r");
  if (file == NULL)
  {
    printf("Erro ao carregar.\n");
    return;
  }

  while (!feof(file))
  {
    int tipo, numPts;
    if (fscanf(file, "%d %d", &tipo, &numPts) != 2)
    {
      break;
    }

    criarNovaForma();
    ultimo->tipo = tipo;
    ultimo->finalizado = 1;

    for (int i = 0; i < numPts; i++)
    {
      float x, y, r, g, b;
      if (fscanf(file, "%f %f %f %f %f", &x, &y, &r, &g, &b) != 5)
      {
        break;
      }

      ponto *novoPonto = (ponto *)malloc(sizeof(ponto));
      if (novoPonto == NULL)
      {
        printf("Falha ao alocar ponto.\n");
        exit(EXIT_FAILURE);
      }

      novoPonto->mat[0][0] = x;
      novoPonto->mat[1][0] = y;
      novoPonto->mat[2][0] = 1.0f;
      novoPonto->cor[0] = r;
      novoPonto->cor[1] = g;
      novoPonto->cor[2] = b;
      novoPonto->cor[3] = 1.0f;
      novoPonto->prox = NULL;

      if (ultimo->pts == NULL)
      {
        ultimo->pts = novoPonto;
        ultimo->ultimoPt = novoPonto;
      }
      else
      {
        ultimo->ultimoPt->prox = novoPonto;
        ultimo->ultimoPt = novoPonto;
      }
      ultimo->numPts++;
    }
  }
}

forma *checaLinha(int x, int y, forma *formaAtual)
{
  if (formaAtual->pts == NULL || formaAtual->pts->prox == NULL)
    return NULL;

  int xMin = x - 10;
  int xMax = x + 10;
  int yMin = y - 10;
  int yMax = y + 10;

  float primVert[2] = {formaAtual->pts->mat[0][0], formaAtual->pts->mat[1][0]};
  float segVert[2] = {formaAtual->pts->prox->mat[0][0], formaAtual->pts->prox->mat[1][0]};

  int res[4] = {0, 0, 0, 0}, pos1[4] = {0, 0, 0, 0}, pos2[4] = {0, 0, 0, 0}; // 0 esq 1 dir 2 acima 3 abaixo

  pos2[0] = (segVert[0] < xMin);
  pos2[1] = (segVert[0] > xMax);
  pos2[2] = (segVert[1] > yMax);
  pos2[3] = (segVert[1] < yMin);

  do
  {

    pos1[0] = (primVert[0] < xMin);
    pos1[1] = (primVert[0] > xMax);
    pos1[2] = (primVert[1] > yMax);
    pos1[3] = (primVert[1] < yMin);

    printf("pos1: %d %d %d %d\n", pos1[0], pos1[1], pos1[2], pos1[3]);
    printf("pos2: %d %d %d %d\n", pos2[0], pos2[1], pos2[2], pos2[3]);

    // se algum dos vertices for 0000
    if ((pos1[0] == 0 && pos1[1] == 0 && pos1[2] == 0 && pos1[3] == 0) || (pos2[0] == 0 && pos2[1] == 0 && pos2[2] == 0 && pos2[3] == 0))
    {
      return formaAtual;
    }
    for (int i = 0; i < 4; i++)
    {
      res[i] = pos1[i] & pos2[i];
    }

    if (res[0] == 1 || res[1] == 1 || res[2] == 1 || res[3] == 1)
      return NULL;

    if (pos1[0] == 1)
    {
      primVert[1] = primVert[1] + (xMin - primVert[0]) * (segVert[1] - primVert[1]) / (segVert[0] - primVert[0]);
      primVert[0] = xMin;
    }
    else if (pos1[1] == 1)
    {
      primVert[1] = primVert[1] + (xMax - primVert[0]) * (segVert[1] - primVert[1]) / (segVert[0] - primVert[0]);
      primVert[0] = xMax;
    }
    else if (pos1[2] == 1)
    {
      primVert[0] = primVert[0] + (yMax - primVert[1]) * (segVert[0] - primVert[0]) / (segVert[1] - primVert[1]);
      primVert[1] = yMax;
    }
    else if (pos1[3] == 1)
    {
      primVert[0] = primVert[0] + (yMin - primVert[1]) * (segVert[0] - primVert[0]) / (segVert[1] - primVert[1]);
      primVert[1] = yMin;
    }

  } while (res[0] == 0 && res[1] == 0 && res[2] == 0 && res[3] == 0);
  return NULL;
}

float calcAngulo(ponto *ponto1, ponto *ponto2)
{
  return atan2(ponto2->mat[1][0] - ponto1->mat[1][0], ponto2->mat[0][0] - ponto1->mat[0][0]);
}

int estaAEsquerda(ponto *ponto1, ponto *ponto2, ponto *ponto3)
{
  // calcula o determinante da matriz formada pelos vetores ponto2 - ponto1 e ponto3 - ponto1
  float determinante = (ponto2->mat[0][0] - ponto1->mat[0][0]) * (ponto3->mat[1][0] - ponto1->mat[1][0]) -
                       (ponto2->mat[1][0] - ponto1->mat[1][0]) * (ponto3->mat[0][0] - ponto1->mat[0][0]);

  return determinante > 0;
}

void varreduraGraham()
{
  if (formaSelecionada == NULL)
    return;

  // ponto mais baixo
  ponto *ponto_de_partida = NULL;

  ponto *pontoAtual = formaSelecionada->pts;
  while (pontoAtual != NULL)
  {
    if (ponto_de_partida == NULL ||
        pontoAtual->mat[1][0] < ponto_de_partida->mat[1][0] ||
        (pontoAtual->mat[1][0] == ponto_de_partida->mat[1][0] && pontoAtual->mat[0][0] < ponto_de_partida->mat[0][0]))
    {
      ponto_de_partida = pontoAtual;
    }
    pontoAtual = pontoAtual->prox;
  }

  if (ponto_de_partida == NULL)
    return;

  // ordenar pelo ângulo

  ponto *pontosOrdenados[1000];
  float angulos[1000];
  int totalPontos = 0;
  pontoAtual = formaSelecionada->pts;
  while (pontoAtual != NULL)
  {
    if (pontoAtual != ponto_de_partida)
    {
      pontosOrdenados[totalPontos] = pontoAtual;
      angulos[totalPontos] = calcAngulo(ponto_de_partida, pontoAtual);
      totalPontos++;
    }
    pontoAtual = pontoAtual->prox;
  }

  for (int i = 0; i < totalPontos - 1; i++)
  {
    for (int j = i + 1; j < totalPontos; j++)
    {
      if (angulos[i] > angulos[j])
      {
        float tempAngulo = angulos[i];
        angulos[i] = angulos[j];
        angulos[j] = tempAngulo;

        ponto *tempPonto = pontosOrdenados[i];
        pontosOrdenados[i] = pontosOrdenados[j];
        pontosOrdenados[j] = tempPonto;
      }
    }
  }

  // construir a envoltoria
  ponto *envoltoria[1000];
  int tamEnvoltoria = 0;

  envoltoria[tamEnvoltoria++] = ponto_de_partida;
  envoltoria[tamEnvoltoria++] = pontosOrdenados[0];

  for (int i = 1; i < totalPontos; i++)
  {
    while (tamEnvoltoria >= 2 &&
           !estaAEsquerda(envoltoria[tamEnvoltoria - 2], envoltoria[tamEnvoltoria - 1], pontosOrdenados[i]))
    {
      tamEnvoltoria--;
    }
    envoltoria[tamEnvoltoria++] = pontosOrdenados[i];
  }

  formaSelecionada->pts = NULL;
  formaSelecionada->ultimoPt = NULL;
  formaSelecionada->numPts = 0;

  for (int i = 0; i < tamEnvoltoria; i++)
  {
    ponto *novoPonto = (ponto *)malloc(sizeof(ponto));
    if (novoPonto == NULL)
    {
      printf("Falha ao alocar\n");
    }

    novoPonto->mat[0][0] = envoltoria[i]->mat[0][0];
    novoPonto->mat[1][0] = envoltoria[i]->mat[1][0];
    novoPonto->mat[2][0] = 1.0f;
    novoPonto->cor[0] = envoltoria[i]->cor[0];
    novoPonto->cor[1] = envoltoria[i]->cor[1];
    novoPonto->cor[2] = envoltoria[i]->cor[2];
    novoPonto->cor[3] = envoltoria[i]->cor[3];
    novoPonto->prox = NULL;

    if (formaSelecionada->pts == NULL)
    {
      formaSelecionada->pts = novoPonto;
      formaSelecionada->ultimoPt = novoPonto;
    }
    else
    {
      formaSelecionada->ultimoPt->prox = novoPonto;
      formaSelecionada->ultimoPt = novoPonto;
    }
    formaSelecionada->numPts++;
  }
}

forma *checaPoligono(int x, int y, forma *formaAtual)
{

  int intersecoes = 0, xi = 0;

  for (ponto *pontoAtual = formaAtual->pts; pontoAtual != NULL; pontoAtual = pontoAtual->prox)
  {

    int primPt[2] = {pontoAtual->mat[0][0], pontoAtual->mat[1][0]};
    int segPt[2] = {0, 0};
    int terPt[2] = {0, 0};

    if (pontoAtual->prox != NULL)
    {
      segPt[0] = pontoAtual->prox->mat[0][0];
      segPt[1] = pontoAtual->prox->mat[1][0];
    }
    else
    {
      segPt[0] = formaAtual->pts->mat[0][0];
      segPt[1] = formaAtual->pts->mat[1][0];
    }

    if (pontoAtual->prox != NULL && pontoAtual->prox->prox != NULL)
    {
      terPt[0] = pontoAtual->prox->prox->mat[0][0];
      terPt[1] = pontoAtual->prox->prox->mat[1][0];
    }
    else
    {
      terPt[0] = formaAtual->pts->mat[0][0];
      terPt[1] = formaAtual->pts->mat[1][0];
    }

    // casos triviais
    if ((primPt[1] > y) && (segPt[1] > y))
      continue;

    if ((primPt[1] < y) && (segPt[1] < y))
      continue;

    if ((primPt[0] < x) && (segPt[0] < x))
      continue;

    if ((primPt[1] == y) && (segPt[1] == y))
      continue;

    if (segPt[1] == y)
    {
      if (primPt[1] < y)
        intersecoes++;
      if (terPt[1] < y)
        intersecoes++;
    }

    if ((primPt[0] > x && segPt[0] > x) && ((primPt[1] > y && segPt[1] < y) || (primPt[1] < y && segPt[1] > y)))
    {
      intersecoes++;
      continue;
    }

    xi = primPt[0] + (y - primPt[1]) * (segPt[0] - primPt[0]) / (segPt[1] - primPt[1]);

    // se a intersecção for menor que x, então o tiro não intercepta, caso contrário, intercepta
    if (xi > x)
      intersecoes++;
  }

  printf("intersecoes: %d\n", intersecoes);

  if (intersecoes % 2 == 1)
  {
    return formaAtual;
  }
  else
  {
    return NULL;
  }
}

forma *checaSelecao(int x, int y)
{
  forma *ret = NULL;
  for (forma *formaAtual = primeiro; formaAtual != NULL && formaAtual->pts != NULL; formaAtual = formaAtual->prox)
  {
    // verificação do ponto
    if (formaAtual->tipo == 0)
    {

      if (x <= formaAtual->pts->mat[0][0] + 5 && x >= formaAtual->pts->mat[0][0] - 5 && y <= formaAtual->pts->mat[1][0] + 5 && y >= formaAtual->pts->mat[1][0] - 5)
      {
        return formaAtual;
      }
    }

    // verificação da linha
    if (formaAtual->tipo == 1)
    {
      ret = checaLinha(x, y, formaAtual);
      if (ret != NULL)
      {
        return ret;
      }
    }

    // verificação do poligono

    if (formaAtual->tipo == 2)
    {
      ret = checaPoligono(x, y, formaAtual);
      if (ret != NULL)
      {
        return ret;
      }
    }
  }
  return NULL;
}

void intro()
{
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(powf((clock()) / 7000.0f, 6.0f), powf((float)(clock()) / 3000.0f, 2.0f), powf((float)(clock()) / 5000.0f, 2.0f), 1.0f);
  glBegin(GL_POLYGON);

  float angulo = powf((float)(clock()) / 6000.0f, 5.0f);
  float xCentral = 600.0f;
  float yCentral = 450.0f;
  float raio = 300.0f - powf((float)(clock()) / 4000.0f, 8.2f);

  glColor3f(1.0f, 0.0f, 0.0f);
  glVertex2f(xCentral + raio * cos(angulo), yCentral + raio * sin(angulo));

  glVertex2f(xCentral + raio * cos(angulo + 3.1415 / 2), yCentral + raio * sin(angulo + 3.1415 / 2));
  glVertex2f(xCentral, yCentral);
  glColor3f(0.0f, 0.0f, 1.0f);
  glVertex2f(xCentral + raio * cos(angulo + 3.1415), yCentral + raio * sin(angulo + 3.1415));
  glColor3f(0.0f, 0.0f, 0.3f);
  glVertex2f(xCentral + raio * cos(angulo + 3.1415), yCentral + raio * sin(angulo + 3.1415));
  glColor3f(0.0f, 0.0f, 1.0f);

  glVertex2f(xCentral + raio * cos(angulo + 3 * 3.1415 / 2), yCentral + raio * sin(angulo + 3 * 3.1415 / 2));

  glEnd();

  glColor3f(0.5f, 0.0f, 1.0f);
  glRasterPos2f(50.0f, 50.0f);
  const char *text = "SPACE PARA PULAR";
  for (const char *c = text; *c != '\0'; c++)
  {
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
  }
  glFlush();
}
void calcularCentro(float *xCentral, float *yCentral)
{
  int Xmax = 0, Xmin = 0, Ymax = 0, Ymin = 0;
  if (formaSelecionada == NULL)
    return;
  if (formaSelecionada->tipo == 0)
  {
    *xCentral = 600.0f;
    *yCentral = 450.0f;
    return;
  }

  ponto *pontoAtual = formaSelecionada->pts;
  Ymax = pontoAtual->mat[1][0];
  Ymin = pontoAtual->mat[1][0];
  Xmax = pontoAtual->mat[0][0];
  Xmin = pontoAtual->mat[0][0];

  for (; pontoAtual != NULL; pontoAtual = pontoAtual->prox)
  {
    if (pontoAtual->mat[0][0] > Xmax)
      Xmax = pontoAtual->mat[0][0];
    if (pontoAtual->mat[0][0] < Xmin)
      Xmin = pontoAtual->mat[0][0];
    if (pontoAtual->mat[1][0] > Ymax)
      Ymax = pontoAtual->mat[1][0];
    if (pontoAtual->mat[1][0] < Ymin)
      Ymin = pontoAtual->mat[1][0];
  }

  *xCentral = (Xmax - Xmin) / 2.0f + Xmin;
  *yCentral = (Ymax - Ymin) / 2.0f + Ymin;
}
void rotacionar(float angulo)
{
  int Xmax = 0, Xmin = 0, Ymax = 0, Ymin = 0;
  if (formaSelecionada == NULL)
    return;

  float xCentral = 0.0f, yCentral = 0.0f;
  calcularCentro(&xCentral, &yCentral);

  float matrizRotacaoComposta[3][3] = {
      {cos(angulo), -sin(angulo), xCentral * (1 - cos(angulo)) + yCentral * sin(angulo)},
      {sin(angulo), cos(angulo), yCentral * (1 - cos(angulo)) - xCentral * sin(angulo)},
      {0.0f, 0.0f, 1.0f}};

  for (ponto *pontoAtual = formaSelecionada->pts; pontoAtual != NULL; pontoAtual = pontoAtual->prox)
  {
    float res[3][1];
    multiplicaMatriz(matrizRotacaoComposta, pontoAtual->mat, res);
    pontoAtual->mat[0][0] = res[0][0];
    pontoAtual->mat[1][0] = res[1][0];
  }
}
void cisalhamento(int direcaoY, int direcaoX)
{

  if (formaSelecionada == NULL)
    return;

  float xCentral = 0.0f, yCentral = 0.0f;
  calcularCentro(&xCentral, &yCentral);

  float matrizCisalhamentoComposta[3][3] = {
      {1.0f, direcaoX * 0.1f, -direcaoX * 0.1f * yCentral},
      {direcaoY * 0.1f, 1.0f, -direcaoY * 0.1f * xCentral},
      {0.0f, 0.0f, 1.0f}};

  for (ponto *pontoAtual = formaSelecionada->pts; pontoAtual != NULL; pontoAtual = pontoAtual->prox)
  {
    float res[3][1];
    multiplicaMatriz(matrizCisalhamentoComposta, pontoAtual->mat, res);
    pontoAtual->mat[0][0] = res[0][0];
    pontoAtual->mat[1][0] = res[1][0];
  }
}
void escala(float fatorX, float fatorY)
{
  if (formaSelecionada == NULL)
    return;
  float xCentral = 0.0f, yCentral = 0.0f;
  calcularCentro(&xCentral, &yCentral);

  float matrizEscalaComposta[3][3] = {
      {fatorX, 0.0f, xCentral * (1.0f - fatorX)},
      {0.0f, fatorY, yCentral * (1.0f - fatorY)},
      {0.0f, 0.0f, 1.0f}};
  for (ponto *pontoAtual = formaSelecionada->pts; pontoAtual != NULL; pontoAtual = pontoAtual->prox)
  {
    float res[3][1];
    multiplicaMatriz(matrizEscalaComposta, pontoAtual->mat, res);
    pontoAtual->mat[0][0] = res[0][0];
    pontoAtual->mat[1][0] = res[1][0];
  }
}
void translacao(float x, float y)
{
  if (formaSelecionada == NULL)
    return;

  float matTranslacao[3][3] = {
      {1.0f, 0.0f, x},
      {0.0f, 1.0f, y},
      {0.0f, 0.0f, 1.0f}};

  for (ponto *pontoAtual = formaSelecionada->pts; pontoAtual != NULL; pontoAtual = pontoAtual->prox)
  {
    float res[3][1];
    multiplicaMatriz(matTranslacao, pontoAtual->mat, res);
    pontoAtual->mat[0][0] = res[0][0];
    pontoAtual->mat[1][0] = res[1][0];
  }
}
int movendoForma = 0;
void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT);

  glPointSize(5.0);
  glLineWidth(3.0f);
  if (movendoForma)
  {
    glutSetCursor(GLUT_CURSOR_CYCLE);
  }
  else if (modoSelecao == 1)
  {
    glutSetCursor(GLUT_CURSOR_INFO);
  }
  else
  {
    glutSetCursor(GLUT_CURSOR_CROSSHAIR);
  }
  glColor4f(corSelec[0], corSelec[1], corSelec[2], corSelec[3]);
  if (modoSelecao == 1)
  {
    glColor3f(.5f, .5f, 0.0f); //
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(2, 0xAAAA);
    glBegin(GL_LINE_LOOP);
    glVertex2f(10.0f, 880.0f);
    glVertex2f(50.0f, 880.0f);
    glVertex2f(50.0f, 840.0f);
    glVertex2f(10.0f, 840.0f);
    glEnd();
    glDisable(GL_LINE_STIPPLE);
  }
  else if (tipoSelec == 1)
  {
    glBegin(GL_LINES);
    glVertex2f(10.0f, 880.0f);
    glVertex2f(50.0f, 840.0f);
    glEnd();
  }
  else if (tipoSelec == 2)
  {
    glBegin(GL_POLYGON);
    glVertex2f(15.0f, 880.0f);
    glVertex2f(45.0f, 875.0f);
    glVertex2f(40.0f, 860.0f);
    glVertex2f(50.0f, 840.0f);
    glVertex2f(10.0f, 850.0f);
    glEnd();
  }
  else if (tipoSelec == 0)
  {
    glBegin(GL_POINTS);
    glVertex2f(30.0f, 860.0f);
    glEnd();
  }

  forma *formaAtual = primeiro;
  while (formaAtual != NULL)
  {
    if (formaSelecionada == formaAtual)
    {
      glPointSize(10.0);
      glEnable(GL_LINE_STIPPLE);
      glLineStipple(2, 0xAAAA);
      glLineWidth(6.0f);
    }

    ponto *pontoAtual = formaAtual->pts;
    if (formaAtual->tipo == 0)
    {
      glBegin(GL_POINTS);
    }
    else if (formaAtual->tipo == 1)
    {
      glBegin(GL_LINES);
    }
    else
    {
      glBegin(GL_LINE_LOOP);
    }
    while (pontoAtual != NULL)
    {
      if (formaSelecionada == formaAtual)
      {
        glColor4f(1.0f, 0.6f, 0.0f, 1.0f);
      }
      else
      {
        glColor4f(pontoAtual->cor[0], pontoAtual->cor[1], pontoAtual->cor[2], pontoAtual->cor[3]);
      }
      glVertex2f(pontoAtual->mat[0][0], pontoAtual->mat[1][0]);
      pontoAtual = pontoAtual->prox;
    }
    glEnd();

    if (formaSelecionada == formaAtual)
    {
      glPointSize(5.0);
      glLineWidth(3.0f);
      glDisable(GL_LINE_STIPPLE);
    }

    formaAtual = formaAtual->prox;
  }

  if (selecionandoCor)
  {
    int larguraPaleta = 400;
    int alturaPaleta = 300;
    glBegin(GL_QUAD_STRIP);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2i(0, 0);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2i(0, alturaPaleta);
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex2i(larguraPaleta / 6, 0);
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex2i(larguraPaleta / 6, alturaPaleta);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2i(2 * larguraPaleta / 6, 0);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2i(2 * larguraPaleta / 6, alturaPaleta);
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex2i(3 * larguraPaleta / 6, 0);
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex2i(3 * larguraPaleta / 6, alturaPaleta);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex2i(4 * larguraPaleta / 6, 0);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex2i(4 * larguraPaleta / 6, alturaPaleta);
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex2i(5 * larguraPaleta / 6, 0);
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex2i(5 * larguraPaleta / 6, alturaPaleta);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2i(larguraPaleta, 0);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2i(larguraPaleta, alturaPaleta);
    glEnd();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);

    glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
    glVertex2i(larguraPaleta, 0);
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
    glVertex2i(0, 0);
    glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    glVertex2i(0, alturaPaleta);

    glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    glVertex2i(larguraPaleta, alturaPaleta);
    glEnd();
    glDisable(GL_BLEND);
  }
  glColor3f(0.0f, 0.0f, 0.0f);
  glRasterPos2f(70.0f, 865.0f);
  const char *text = "'f' cor, 'c' salvar, 'v' carregar, 'n' limpar, 'b' carregar limpo, 'i/j/k/l' cisalhamento, 'r/t' reflexao, 'q/e' rotacao, 'Backspace' ou 'DEL' deletar,";
  for (const char *c = text; *c != '\0'; c++)
  {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
  }
  glColor3f(0.0f, 0.0f, 0.0f);
  glRasterPos2f(70.0f, 835.0f);
  const char *text2 = "'w/a/s/d/' ou 'clique e arraste' translacao, '1/2/3' formas, 'z' desfazer, 'TAB' selecionar, 'g' Graham, M3 modo de escala";
  for (const char *c = text2; *c != '\0'; c++)
  {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
  }

  if (clock() < 8000 && pularIntro == 0)
  {
    intro();
    glutTimerFunc(16, (void (*)(int))glutPostRedisplay, 0);
  }
  glFlush();
}
void deletarTudo()
{
  forma *formaAtual = primeiro;
  while (formaAtual != NULL)
  {
    forma *nextShape = formaAtual->prox;
    deletarForma(formaAtual);
    formaAtual = nextShape;
  }
  primeiro = NULL;
  ultimo = NULL;
  formaSelecionada = NULL;
}

void keyboard(unsigned char key, int x, int y)
{
  if (key == ' ')
  {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    pularIntro = 1;
    return;
  }
  if (key == 'r')
  {
    escala(-1.0f, 1);
    glutPostRedisplay();
  }
  if (key == 't')
  {
    escala(1, -1.0f);
    glutPostRedisplay();
  }
  if (key == 'b')
  {
    deletarTudo();
    carregar();
    glutPostRedisplay();
  }
  if (key == 'n')
  {
    deletarTudo();
    glutPostRedisplay();
  }
  if (key == 'c')
  {
    salvar();
    glutPostRedisplay();
  }
  if (key == 'g')
  {
    if (formaSelecionada != NULL && formaSelecionada->tipo == 2) // Verifica se é um polígono
    {
      varreduraGraham();
      glutPostRedisplay();
    }
  }
  if (key == 'v')
  {
    carregar();
    glutPostRedisplay();
  }
  if (key == 'i')
  {
    if (formaSelecionada != NULL && formaSelecionada->tipo != 0)
    {
      cisalhamento(1, 0);
      glutPostRedisplay();
    }
  }
  if (key == 'k')
  {
    if (formaSelecionada != NULL && formaSelecionada->tipo != 0)
    {
      cisalhamento(-1, 0);
      glutPostRedisplay();
    }
  }
  if (key == 'l')
  {
    if (formaSelecionada != NULL && formaSelecionada->tipo != 0)
    {
      cisalhamento(0, 1);
      glutPostRedisplay();
    }
  }
  if (key == 'j')
  {
    if (formaSelecionada != NULL && formaSelecionada->tipo != 0)
    {
      cisalhamento(0, -1);
      glutPostRedisplay();
    }
  }

  if (key == 'w')
  {
    translacao(0.0f, 10.0f);
    glutPostRedisplay();
  }
  if (key == 's')
  {
    translacao(0.0f, -10.0f);
    glutPostRedisplay();
  }
  if (key == 'a')
  {
    translacao(-10.0f, 0.0f);
    glutPostRedisplay();
  }
  if (key == 'd')
  {
    translacao(10.0f, 0.0f);
    glutPostRedisplay();
  }
  if (key == 'q')
  {
    rotacionar(0.1f);
    glutPostRedisplay();
  }
  if (key == 'e')
  {
    rotacionar(-0.1f);
    glutPostRedisplay();
  }
  if (key == 8 || key == 127) // backspace ou delete
  {
    deletarForma(formaSelecionada);
    glutPostRedisplay();
  }
  if (key == 'z')
  {
    deletarForma(ultimo);
    glutPostRedisplay();
  }
  if (key == 'f')
  {
    selecionandoCor = !selecionandoCor;
    glutPostRedisplay();
  }
  if (key == 9) // tab
  {
    formaSelecionada = NULL;
    modoSelecao = !modoSelecao;
    glutPostRedisplay();
  }
  if (key >= '1' && key <= '3')
  {
    tipoSelec = key - '1';
    formaSelecionada = NULL;
    modoSelecao = 0;
    glutPostRedisplay();

    criarNovaForma();
  }
}
int modoEscala = 0;

int ix = 0;
int iy = 0;
void mouse(int button, int state, int x, int y)
{
  // inverte de cara o y
  y = glutGet(GLUT_WINDOW_HEIGHT) - y;
  y = (y * 900) / glutGet(GLUT_WINDOW_HEIGHT);
  x = (x * 1200) / glutGet(GLUT_WINDOW_WIDTH);
  printf("Mouse: %d %d %d %d\n", button, state, x, y);

  if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
  {
    modoEscala = (modoEscala + 1) % 3;
    if (modoEscala == 0)
    {
      glutSetCursor(GLUT_CURSOR_CYCLE);
    }
    if (modoEscala == 1)
    {
      glutSetCursor(GLUT_CURSOR_LEFT_RIGHT);
    }
    if (modoEscala == 2)
    {
      glutSetCursor(GLUT_CURSOR_UP_DOWN);
    }
  }

  if (button == 4)
  {
    if (modoEscala == 0)
    {
      escala(1.0f / 1.1f, 1.0f / 1.1f);
    }
    if (modoEscala == 1)
    {
      escala(1.0f / 1.1f, 1.0f);
    }
    if (modoEscala == 2)
    {
      escala(1.0f, 1.0f / 1.1f);
    }

    glutPostRedisplay();
  }
  if (button == 3)
  {
    if (modoEscala == 0)
    {
      escala(1.1f, 1.1f);
    }
    if (modoEscala == 1)
    {
      escala(1.1f, 1.0f);
    }
    if (modoEscala == 2)
    {
      escala(1.0f, 1.1f);
    }
    glutPostRedisplay();
  }
  if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
  {
    criarNovaForma();
    glutPostRedisplay();
  }

  if (button == GLUT_LEFT_BUTTON && state == GLUT_UP && movendoForma)
  {
    movendoForma = 0;
    return;
  }

  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
  {
    if (formaSelecionada != NULL && !movendoForma)
    {
      movendoForma = 1;
      ix = x;
      iy = y;
    }
    if (selecionandoCor)
    {

      int larguraPaleta = 400;
      int alturaPaleta = 300;

      if (x < larguraPaleta && y < alturaPaleta)
      {

        float nX = (float)x / larguraPaleta;
        float nY = (float)y / alturaPaleta;
        float red, green, blue;
        if (nX < 1.0f / 6.0f)
        {
          red = nY * 1.0f;
          green = nY * (nX * 6.0f);
          blue = 0.0f;
        }
        else if (nX < 2.0f / 6.0f)
        {
          red = nY * (1.0f - (nX - 1.0f / 6.0f) * 6.0f);
          green = nY * 1.0f;
          blue = 0.0f;
        }
        else if (nX < 3.0f / 6.0f)
        {
          red = 0.0f;
          green = nY * 1.0f;
          blue = nY * ((nX - 2.0f / 6.0f) * 6.0f);
        }
        else if (nX < 4.0f / 6.0f)
        {
          red = 0.0f;
          green = nY * (1.0f - (nX - 3.0f / 6.0f) * 6.0f);
          blue = nY * 1.0f;
        }
        else if (nX < 5.0f / 6.0f)
        {
          red = nY * ((nX - 4.0f / 6.0f) * 6.0f);
          green = 0.0f;
          blue = nY * 1.0f;
        }
        else
        {
          red = nY * 1.0f;
          green = 0.0f;
          blue = nY * (1.0f - (nX - 5.0f / 6.0f) * 6.0f);
        }

        corSelec[0] = red;
        corSelec[1] = green;
        corSelec[2] = blue;

        printf("Cor selecionada: R=%f, G=%f, B=%f\n", red, green, blue);
        return;
      }
    }
    if (modoSelecao)
    {
      formaSelecionada = checaSelecao(x, y);
      return;
    }

    if (ultimo == NULL || ((ultimo->tipo == 0 && ultimo->numPts >= 1) || (ultimo->tipo == 1 && ultimo->numPts >= 2) || ultimo->finalizado))
    {
      criarNovaForma();
    }

    ponto *novoPonto = (ponto *)malloc(sizeof(ponto));
    if (novoPonto == NULL)
    {
      printf("Falha ao alocar.\n");
    }

    novoPonto->mat[0][0] = (float)x;
    novoPonto->mat[1][0] = (float)y;
    novoPonto->mat[2][0] = 1.0f;
    novoPonto->cor[0] = corSelec[0];
    novoPonto->cor[1] = corSelec[1];
    novoPonto->cor[2] = corSelec[2];
    novoPonto->cor[3] = corSelec[3];
    novoPonto->prox = NULL;

    if (ultimo != NULL)
    {
      if (ultimo->pts == NULL)
      {
        ultimo->pts = novoPonto;
        ultimo->ultimoPt = novoPonto;
      }
      else
      {
        ultimo->ultimoPt->prox = novoPonto;
        ultimo->ultimoPt = novoPonto;
      }
      ultimo->numPts++;
    }
    glutPostRedisplay();
  }

  printf("Mouse: %d %d %d %d\n", button, state, x, y);
}
void mousePos(int x, int y)
{
  if (movendoForma && formaSelecionada != NULL)
  {
    // inverte de cara o y
    y = glutGet(GLUT_WINDOW_HEIGHT) - y;
    y = (y * 900) / glutGet(GLUT_WINDOW_HEIGHT);
    x = (x * 1200) / glutGet(GLUT_WINDOW_WIDTH);
    translacao(x - ix, y - iy);
    ix = x;
    iy = y;
    glutTimerFunc(16, (void (*)(int))glutPostRedisplay, 0);
  }
}

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(1200, 900);
  glutInitWindowPosition(100, 0);
  glutCreateWindow("Paint");

  glClearColor(1.0, 1.0, 1.0, 0.0);
  glMatrixMode(GL_PROJECTION);
  gluOrtho2D(0.0, 1200.0, 0.0, 900.0);

  glutKeyboardFunc(keyboard);

  glutMouseFunc(mouse);
  glutMotionFunc(mousePos);

  glutDisplayFunc(display);
  glutMainLoop();

  return 0;
}
