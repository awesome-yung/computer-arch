/****************************************/
/*                                      */
/*                                      */
/*  Game of Life with Pthread and CUDA  */
/*                                      */
/*  ECE521 Project #2                   */
/*  @ Korea University                  */
/*                                      */
/*                                      */
/****************************************/

#include "glife.h"
using namespace std;

int gameOfLife(int argc, char *argv[]);
void singleThread(int, int, int);
void* workerThread(void *);
int nprocs;
GameOfLifeGrid* g_GameOfLifeGrid;
struct thread_data {
    int from;
    int to;
    GameOfLifeGrid* grid;
  };

uint64_t dtime_usec(uint64_t start)
{
  timeval tv;
  gettimeofday(&tv, 0);
  return ((tv.tv_sec*USECPSEC)+tv.tv_usec)-start;
}

GameOfLifeGrid::GameOfLifeGrid(int rows, int cols, int gen)
{
  m_Generations = gen;
  m_Rows = rows;
  m_Cols = cols;

  m_Grid = (int**)malloc(sizeof(int*) * rows);
  if (m_Grid == NULL) 
    cout << "1 Memory allocation error " << endl;

  m_Temp = (int**)malloc(sizeof(int*) * rows);
  if (m_Temp == NULL) 
    cout << "2 Memory allocation error " << endl;

  m_Grid[0] = (int*)malloc(sizeof(int) * (cols*rows));
  if (m_Grid[0] == NULL) 
    cout << "3 Memory allocation error " << endl;

  m_Temp[0] = (int*)malloc(sizeof(int) * (cols*rows));	
  if (m_Temp[0] == NULL) 
    cout << "4 Memory allocation error " << endl;

  for (int i = 1; i < rows; i++) {
    m_Grid[i] = m_Grid[i-1] + cols;
    m_Temp[i] = m_Temp[i-1] + cols;
  }

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      m_Grid[i][j] = m_Temp[i][j] = 0;
    }
  }
}

// Entry point
int main(int argc, char* argv[])
{
  if (argc != 7) {
    cout <<"Usage: " << argv[0] << " <input file> <display> <nprocs>"
           " <# of generation> <width> <height>" << endl;
    cout <<"\n\tnprocs = 0: Enable GPU" << endl;
    cout <<"\tnprocs > 0: Run on CPU" << endl;
    cout <<"\tdisplay = 1: Dump results" << endl;
    return 1;
  }

  return gameOfLife(argc, argv);
}

int gameOfLife(int argc, char* argv[])
{
  int cols, rows, gen;
  ifstream inputFile;
  int input_row, input_col, display;
  uint64_t difft, cuda_difft;
  pthread_t *threadID;
  

  inputFile.open(argv[1], ifstream::in);

  if (inputFile.is_open() == false) {
    cout << "The "<< argv[1] << " file can not be opend" << endl;
    return 1;
  }

  display = atoi(argv[2]);
  nprocs = atoi(argv[3]);
  gen = atoi(argv[4]);
  cols = atoi(argv[5]);
  rows = atoi(argv[6]);

  g_GameOfLifeGrid = new GameOfLifeGrid(rows, cols, gen);

  while (inputFile.good()) {
    inputFile >> input_row >> input_col;
    if (input_row >= rows || input_col >= cols) {
      cout << "Invalid grid number" << endl;
      return 1;
    } else
      g_GameOfLifeGrid->setCell(input_row, input_col);
  }

  // Start measuring execution time
  difft = dtime_usec(0);

  // TODO: YOU NEED TO IMPLMENT THE SINGLE THREAD, PTHREAD, AND CUDA
  if (nprocs == 0) {
    // Running on GPU
    // cuda_difft = runCUDA(rows, cols, gen, g_GameOfLifeGrid, display);
  } else if (nprocs == 1) {
    // Running a single thread
    singleThread(rows, cols, gen);
  } else {
    // Running multiple threads (pthread)
    // void* workerThread(argv);
    // Running multiple threads (pthread)
    pthread_t* threadID = new pthread_t[nprocs];
    thread_data* from_to = new thread_data[nprocs];

    int inst_per_thread = rows / nprocs;
    for (int i = 0; i < nprocs; i++) {
        from_to[i].from = i * inst_per_thread;
        from_to[i].to = (i == nprocs - 1) ? rows : (i + 1) * inst_per_thread;
        from_to[i].grid = g_GameOfLifeGrid;
        pthread_create(&threadID[i], NULL, workerThread, (void*)&from_to[i]);
    }

    for (int g = 0; g < gen; g++) {
        for (int i = 0; i < nprocs; i++) {
            pthread_join(threadID[i], NULL);
        }
        g_GameOfLifeGrid->copyGrid(); // Ensure the grid is copied after all threads have completed
      }

      delete[] threadID;
      delete[] from_to;
  }

  difft = dtime_usec(difft);

  // Print indices only for running on CPU(host).
  if (display && nprocs) {
    // g_GameOfLifeGrid->dump();
    // g_GameOfLifeGrid->dumpIndex();
  }

  if (nprocs) {
    // Single or multi-thread execution time 
    cout << "Execution time(seconds): " << difft/(float)USECPSEC << endl;
  } else {
    // CUDA execution time
    cout << "Execution time(seconds): " << cuda_difft/(float)USECPSEC << endl;
  }
  inputFile.close();
  cout << "Program end... " << endl;
  return 0;
}




// TODO: YOU NEED TO IMPLMENT SINGLE THREAD
void singleThread(int rows, int cols, int gen)
{
  for(int g=0; g<gen;g++)
  {
    g_GameOfLifeGrid->next();
  }
}


// TODO: YOU NEED TO IMPLMENT PTHREAD
void* workerThread(void *arg)
{
    // thread_data 포인터를 받아서 스레드 데이터 설정
    thread_data* from_to = (thread_data*) arg;
    GameOfLifeGrid* grid = from_to->grid;

    for (int g = 0; g < grid->getGens(); g++) {
        // 각 스레드가 처리할 영역을 정의
        for (int line = from_to->from; line < from_to->to; line++) {
            for (int c = 0; c < grid->getCols(); c++) {
                if (grid->isLive(line, c)) {
                    if (2 <= grid->getNumOfNeighbors(line, c) && grid->getNumOfNeighbors(line, c) <= 3) {
                        grid->live(line, c);
                    } else {
                        grid->dead(line, c);
                    }
                } else {
                    if (grid->getNumOfNeighbors(line, c) == 3) {
                        grid->live(line, c);
                    } else {
                        grid->dead(line, c);
                    }
                }
            }
        }
        // 각 세대 후에 그리드를 복사
        grid->copyGrid();
    }

    return NULL;
}

// HINT: YOU MAY NEED TO FILL OUT BELOW FUNCTIONS OR CREATE NEW FUNCTIONS


void GameOfLifeGrid::next()
{
  for(int line=0; line<m_Rows; line++)
  {
    for(int c=0; c<m_Cols; c++)
    {
      if(g_GameOfLifeGrid->isLive(line,c)) // 살았다면
      {
        if(2<=g_GameOfLifeGrid->getNumOfNeighbors(line,c) && g_GameOfLifeGrid->getNumOfNeighbors(line,c)<=3) // 2,3개일때
        {
          g_GameOfLifeGrid->live(line,c);
        }
        else
        {
          g_GameOfLifeGrid->dead(line,c);
        }
      }
      else // 죽었다면
      {
        if(3==g_GameOfLifeGrid->getNumOfNeighbors(line,c)) // 3개 일때
        {
          g_GameOfLifeGrid->live(line,c);
        }
        else
        {
          g_GameOfLifeGrid->dead(line,c);
        }
      }
    }
  }
  g_GameOfLifeGrid->copyGrid();
}

// TODO: YOU MAY NEED TO IMPLMENT IT TO GET NUMBER OF NEIGHBORS 
int GameOfLifeGrid::getNumOfNeighbors(int rows, int cols)
{
  int numOfNeighbors =0;
  for(int line=rows-1; line<rows+2; line++)
  {
    for(int c=cols-1; c<cols+2; c++)
    {
      if (line == rows && c == cols)
        {
          continue;
        }
      if (line >= 0 && line < m_Rows && c >= 0 && c < m_Cols)
        {
          if (isLive(line, c)) 
            {
              ++numOfNeighbors;
            }
        }
    }  
  }
  return numOfNeighbors;
}

void GameOfLifeGrid::dump() 
{
  cout << "===============================" << endl;

  for (int i = 0; i < m_Rows; i++) {
    cout << "[" << i << "] ";
    for (int j = 0; j < m_Cols; j++) {
      if (m_Grid[i][j] == 1)
        cout << "*";
      else
        cout << "o";
    }
    cout << endl;
  }
  cout << "===============================\n" << endl;
}

void GameOfLifeGrid::dumpIndex()
{
  cout << ":: Dump Row Column indices" << endl;
  for (int i=0; i < m_Rows; i++) {
    for (int j=0; j < m_Cols; j++) {
      if (m_Grid[i][j]) cout << i << " " << j << endl;
    }
  }
}
