Projeto de Computação Grafica para criar uma verção simplificada do paint, na linguagem C usando a API do OpenGL

Foi utilizado o MINGW e a biblioteca do FreeGLUT, e o Code Block para codificação e usado os parametro "-lopengl32 -lglu32 -lfreeglut" em 'Project Buid Options -> linker seting -> other linker options' para windows e no Linux é "-lGL -lglut -lGLU"

| **Mouse/Tecla** | **Ação**                                    |
| --------------- | ------------------------------------------- |
| button\_1       | Selecionar e criar o texto                  |
| button\_2       | Inicia uma nova                             |
| button\_3       | Altera o eixo de escala                     |
| scroll          | Escala                                      |
| Tab             | Seleção                                     |
| Backspace       | Deleta o elemento selecionado               |
| 1 a 3           | Define o tipo de forma                      |
| w, a, s, d      | Translação (cima, esquerda, baixo, direita) |
| q, e            | Rotação (horária e anti-horária)            |
| i, k, l, j      | Cisalhamento (diferentes direções)          |
| c               | Salva os elementos                          |
| v               | Carrega os elementos por cima               |
| b               | Limpa e carrega os elementos                |
| n               | Limpa                                       |
| z               | Deleta a última forma                       |
| f               | Ativa/desativa a seleção de cores           |
| g               | Realiza a Varredura de Graham               |
-------------------------------------------------------------------------------------
| **Tecla** | **Ação**                                                               |
| --------- | ---------------------------------------------------------------------- |
| w         | Translação para cima (trans(0.0f, 10.0f))                              |
| a         | Translação para a esquerda (trans(-10.0f, 0.0f))                       |
| s         | Translação para baixo (trans(0.0f, -10.0f))                            |
| d         | Translação para a direita (trans(10.0f, 0.0f))                         |
| q         | Rotação no sentido horário (rotacionar(0.1f))                          |
| e         | Rotação no sentido anti-horário (rotacionar(-0.1f))                    |
| i         | Cisalhamento no eixo X (cisalhamentoY(1, 0))                           |
| k         | Cisalhamento no eixo X (cisalhamentoY(-1, 0))                          |
| l         | Cisalhamento no eixo Y (cisalhamentoY(0, 1))                           |
| j         | Cisalhamento no eixo Y (cisalhamentoY(0, -1))                          |
| r         | Reflexão em X (escala(-1.0f, 1))                                       |
| t         | Reflexão em Y (escala(1, -1.0f))                                       |
| g         | Varredura de Graham (varreduraGraham())                                |
| c         | Salvar elementos (salvar())                                            |
| v         | Recarregar elementos (carregar())                                      |
| b         | Deletar tudo e recarregar (deletarTudo(); carregar())                  |
| n         | Deletar tudo (deletarTudo())                                           |
| Backspace | Deletar a forma selecionada (deleteShape(selectedShape))               |
| Tab       | Ativar/desativar modo de seleção (selectionMode = !selectionMode)      |
| z         | Deletar a última forma (deleteShape(tail))                             |
| f         | Ativar/desativar seleção de cor (isSelectingColor = !isSelectingColor) |
| 1 a 3     | Define o tipo de forma (selectedType = key - '1'; createNextShape())   |
