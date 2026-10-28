# Protocol Purge

> **Jogo 2D de Sobrevivência e Ação em C com Raylib**  
> *Projeto acadêmico premiado em 1º lugar no curso de Ciência da Computação da UERN.*

[![Language: C](https://img.shields.io/badge/Language-C99-blue.svg)](https://en.wikipedia.org/wiki/C99)
[![Library: Raylib](https://img.shields.io/badge/Library-Raylib-red.svg)](https://www.raylib.com/)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Institution: UERN](https://img.shields.io/badge/Institution-UERN-blue.svg)](https://portal.uern.br/)

---

## 📖 1. Contexto Acadêmico

Este jogo foi concebido e desenvolvido no **1º período** do curso de **Ciência da Computação** da **Universidade do Estado do Rio Grande do Norte (UERN)**, como projeto final avaliativo da disciplina de **Introdução à Programação**, ministrada pela **Profa. Ceres Germana Braga Morais**.

### 👥 Autores
- **Ana Júlia de Medeiros Leite**
- **Antonio Carlos Monteiro Silva**
- **Richard Cauê Andrade Matias**

### 🏆 Reconhecimento
O projeto foi apresentado em uma feira/oficina do curso de Ciência da Computação aberta à comunidade acadêmica e avaliado por uma banca composta por docentes do curso juntamente com a professora da disciplina, conquistando o **1º Lugar** entre os projetos desenvolvidos pela turma.

### 🎯 Restrições e Desafio do Projeto
- **Proibição de Game Engines:** Era vedada a utilização de motores de jogos prontos (como Unity, Unreal, Godot, GameMaker, etc.).
- **Linguagem:** Desenvolvimento estritamente em **C** ou **C++** (o grupo optou por **C99**).
- **Recursos Gráficos:** Permitido apenas o uso de bibliotecas gráficas e de multimídia de baixo nível — foi adotada a biblioteca [Raylib](https://www.raylib.com/).

---

## ☣️ 2. Enredo e História

Em um cenário pós-incidente, experimentos biológicos ultrassecretos saíram do controle, desencadeando uma epidemia devastadora que contaminou humanos e gerou hordas de criaturas mutantes agressivas.

O jogador assume o controle de um **soldado de contenção tática**, enviado ao epicentro da infecção com uma única missão: purgar a ameaça biológica. Como armas de fogo convencionais se mostraram ineficazes contra a carapaça das aberrações, o soldado opera com **bombas táticas de alto impacto**, necessitando de agilidade e posicionamento estratégico para detonar os explosivos e conter as ondas contínuas de contaminação.

---

## 🎮 3. Mecânicas e Gameplay

- **Sobrevivência em Rodadas (Rounds):** A cada onda superada, o nível de contaminação cresce, elevando a quantidade máxima de criaturas ativas simultaneamente na tela.
- **Sistema de Power-ups (Level Up):** Ao atingir a cota de experiência (XP) gerada pelos inimigos derrotados, o jogo pausa e oferece uma seleção randômica de 3 melhorias:
  - ⚡ **Velocidade:** Incrementa a agilidade de deslocamento do soldado.
  - ❤️ **Vida:** Restaura e expande a saúde máxima do soldado.
  - 💥 **Dano:** Multiplicador que potencializa o dano de área das bombas.
  - 💣 **Capacidade de Bombas:** Expande o estoque de munição e quantidade de bombas detonadas simultaneamente.
- **Munição Tática:** O soldado possui capacidade limitada de bombas que se recarregam passivamente ao longo do tempo.
- **Mecânica de Escape e Invulnerabilidade:** Sofrer dano garante breve invulnerabilidade visual e um surto temporário de velocidade para permitir a fuga de cercos.
- **Sistema de Ranking:** As 10 melhores marcas de sobrevivência (Nível e Tempo) são salvas em disco de forma persistente.

---

## 🔬 4. Análise Técnica e Arquitetura de Software

Este projeto serve como uma excelente base de estudos para programadores interessados em desenvolvimento de jogos em C puro, demonstrando estruturas de dados, gerenciamento manual de recursos e boas práticas de organização de código.

### 4.1. Estrutura Modular

```text
Game-Protocol-Purge/
├── assets/                  # Texturas, folhas de sprites (spritesheets) e arte
├── build/                   # Artefatos de compilação (.o) e binário executável
├── src/
│   ├── types.h              # Structs globais, enums de estado e constantes centrais
│   ├── asset_manager.c/.h   # Carregamento e descarregamento centralizado de texturas
│   ├── bomb.c/.h            # Lógica, estados e animação dos explosivos
│   ├── enemy.c/.h           # IA de perseguição direta e animação dos inimigos
│   ├── map.c/.h             # Renderização em grid para mapa infinito (tiling)
│   ├── player.c/.h          # Movimentação, entrada de usuário, vida e colisão do herói
│   ├── game.c/.h            # Loop de jogo, spawns, progressão de rounds e ranking
│   └── main.c               # Ponto de entrada, configuração de janela e loop principal
├── Makefile                 # Script de build automatizado via GCC
└── ranking.txt              # Arquivo de persistência do placar de líderes
```

### 4.2. Padrões de Projeto e Técnicas Utilizadas

1. **Máquina de Estados Finita (Finite State Machine - FSM):**
   O fluxo de navegação do jogo é gerenciado via enum `GameState`, alternando entre telas de forma previsível e sem acoplamento (`START_MENU` ➔ `OPTIONS_MENU` ➔ `NAME_INPUT` ➔ `PLAYING` ➔ `LEVEL_UP_OPTIONS` ➔ `DEATH` / `RANKING_VIEW`).
2. **Object Pooling (Reaproveitamento de Entidades):**
   Para evitar alocações e desalocações frequentes durante o gameplay (que causam fragmentação de memória e quedas de frame), o jogo trabalha com limites estáticos pré-alocados para inimigos (`MAX_ENEMIES_CAPACITY = 250`) e bombas (`MAX_BOMBS = 20`), ativando e desativando instâncias conforme a necessidade.
3. **Física e Detecção de Colisão em Camadas:**
   - **Inimigo-Inimigo:** Algoritmo de repulsão elástica baseado em vetores de distância para evitar sobreposição excessiva de sprites da horda.
   - **Inimigo-Jogador:** Detecção por retângulos delimitadores (`CheckCollisionRecs`).
   - **Bomba-Inimigo:** Detecção radial de dano em área (`CheckCollisionCircleRec`) ativada precisamente nos frames de maior impacto visual da animação de explosão.
4. **Animação Baseada em Delta Time:**
   Os spritesheets são percorridos com base no acúmulo de tempo de quadro (`deltaTime`), garantindo consistência na velocidade das animações independente da taxa de quadros.
5. **Persistência com Formato Simples e Algoritmo `qsort`:**
   O ranking lê e grava entradas formatadas em texto plano (`fscanf`/`fprintf`), mantendo os registros sempre organizados utilizando a função `qsort` da biblioteca padrão com um comparador de critérios múltiplos (maior nível, seguido por maior tempo de sobrevivência).

---

## 🕹️ 5. Controles

| Tecla / Comando | Ação |
| :--- | :--- |
| **W, A, S, D** | Movimentação do personagem / Navegação nos menus |
| **E** | Soltar bomba(s) tática(s) |
| **ENTER / ESPAÇO** | Confirmar opções nos menus e seleções de buffs |
| **BACKSPACE** | Apagar caracteres na tela de digitação de nome |
| **R** | Reiniciar a partida após a morte do jogador |
| **ESC** | Retornar ao menu principal a partir do ranking |

---

## 🛠️ 6. Como Compilar e Executar

### Pré-requisitos
- Compilador **GCC** com suporte a C99.
- Biblioteca **Raylib** instalada no sistema (ou inclusa nos paths padrão de bibliotecas).
- Dependências padrão de renderização do Linux: OpenGL, X11, pthread, dl, rt e m.

### Compilação

Para compilar o projeto, execute:
```bash
make
```

Para executar o jogo após compilar:
```bash
make run
```

Para limpar os arquivos objetos e binários compilados:
```bash
make clean
```

---

## 📄 7. Licença

Este projeto é distribuído sob os termos da licença **MIT**. Consulte o arquivo [LICENSE](LICENSE) para mais detalhes.
