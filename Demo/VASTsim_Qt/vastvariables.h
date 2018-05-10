#ifndef VASTVARIABLES_H
#define VASTVARIABLES_H



#include <vector>
#include <map>

// use VASTsim as underlying library
// must set dependency for projects "VASTsim"
#include "VASTsim.h"

// use VASTATEsim as underlying library
// must set dependency for projects "VASTATEsim"
//#include "VASTATEsim.h"

using namespace Vast;
//using namespace std;          //Will possibly need this later, trying to get rid of

#define EN_LEVEL    1

// how often should the timer be called (in millisecond)
#define TIMER_INTERVAL      40

#define SCREEN_DIMENSION_X  800
#define SCREEN_DIMENSION_Y  600

// simulation parameters
VASTPara_Net netpara (VAST_NET_EMULATED);
extern SimPara                     g_para;

extern map<int, vector<Node *> *>   g_nodes;            // map of each node's AOI neighbors
map<int, vector<Vast::id_t> *>      g_ENs;              // map of each node's enclosing neighbors
map<Vast::id_t, int>                g_id2index;         // mapping from node ID to node index
extern int                          g_steps;
//int                         CREATE_NODE_COUNTDOWN = 10;

// GUI-settings
int     NODE_RADIUS     = 10;
bool    step_mode       = true;     // toggle for step_mode
bool    follow_mode     = false;     // toggle for follow_mode
bool    paused          = false;    // toggle for pausing
bool    show_edges      = true;     // display Voronoi edges
bool    show_box        = false;     // display bounding box
bool    show_node_id    = true;     // display node IDs
bool    show_self       = true;     // display my AOI and AOI neighbors in colors
bool    pause_at_incon  = false;     // pause at inconsistency
char    last_char       = 0;        // numerical value of last key pressed
bool    finished        = false;    // simulation is done

QPointF         g_cursor;
QPointF         g_origin;
int             g_selected      = 0;        // currently active node index
int             g_nodes_active  = 0;        // # of active nodes (not failed)
int             g_nodes_created = 0;        // total # of nodes created so far
length_t        g_aoi           = 0;        // aoi of current node
Vast::id_t      g_id            = 0;        // id of current node
int             g_inconsistent  = 0;        // # of inconsistent nodes
map<Vast::id_t, bool> g_show_aoi;                 // flags to whether to draw a node's AOI
int             g_create_countdown = 0;     // countdown in steps to create a new node

#define IDT_TIMER1  77

int wvalue, lvalue;

// check if a particular id is an enclosing neighbor
bool is_EN (vector<Vast::id_t>* en_list, Vast::id_t id)
{
    if (en_list == NULL)
        return false;

    for (size_t i=0; i<en_list->size (); ++i)
        if (en_list->at (i) == id)
            return true;

        return false;
}

// obtain current list of neighbors (to be rendered)
void RefreshNeighbors ()
{
    Node *node;

    for (int i=0; i < g_nodes_created; ++i)
    {
        if ((node = GetNode (i)) != NULL)
        {
            // create mapping & pointers for neighbors & enclosing neighbors
            g_id2index[node->id] = i;
            g_nodes[i] = GetNeighbors (i);
            g_ENs[i] = GetEnclosingNeighbors (i, EN_LEVEL);
        }
    }
}

void MoveOnce ()
{
    if (finished == true)
        return;

    g_steps++;

    // create new nodes
    if (g_nodes_created < g_para.NODE_SIZE)
    {
        if (g_create_countdown == 0)
        {
            // only make sure gateway is fully joined before creating next node
            if (CreateNode (g_nodes_created == 0) == true)
//                if (g_show_aoi.empty()) {
//                    throw "Map is equal to NULL";
//                }
                g_show_aoi.insert(std::pair<Vast::id_t, bool>(g_nodes_created, false));
//                g_show_aoi[g_nodes_created++] = false;

            g_create_countdown = g_para.JOIN_RATE;
        }
        else
            g_create_countdown--;
    }

    g_inconsistent = NextStep ();

    if (g_inconsistent < 0)
    {
        finished = true;
    }

    else if (g_inconsistent > 0 && pause_at_incon)
    {
        step_mode = true;
        paused = true;
    }

    if (!finished)
        RefreshNeighbors ();

//    InvalidateRect (hWnd, NULL, TRUE);        //Force update() after this function
}

//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------

//LRESULT WINAPI MsgProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//    switch (msg)
//    {
//        case WM_DESTROY:
//            ShutSim ();
//            PostQuitMessage (0);
//            break;

//        case WM_PAINT:
//            Render (hWnd);
//            ValidateRect (hWnd, NULL);
//            break;

//        case WM_TIMER:
//            if (paused == false && !finished)
//                MoveOnce (hWnd);
//            if (step_mode == true)
//                paused = true;
//            break;

//        case WM_CHAR:
//            last_char = (char)wParam;
//            // 'space' for next step
//            lvalue = lParam;
//            wvalue = wParam;

//            if (wParam == ' ')
//            {
//                paused = !paused;
//                if (paused == true)
//                    step_mode = true;
//            }
//            // 'enter' for toggling step mode
//            else if (wParam == 13)
//            {
//                step_mode = !step_mode;
//                if (step_mode == false)
//                    paused = false;
//            }
//            // turning on/off Voronoi edges
//            else if (wParam == 'e')
//            {
//                show_edges = !show_edges;
//                InvalidateRect (hWnd, NULL, TRUE);
//            }
//            // turning on/off bounding boxes
//            else if (wParam == 'b')
//            {
//                show_box = !show_box;
//                InvalidateRect (hWnd, NULL, TRUE);
//            }
//            // turning on/off following mode
//            else if (wParam == 'f')
//            {
//                follow_mode = !follow_mode;
//                InvalidateRect (hWnd, NULL, TRUE);
//            }
//            // turning on/off pause at inconsistency
//            else if (wParam == 'c' )
//            {
//                pause_at_incon = !pause_at_incon;
//            }
//            // turning on/off node numbering
//            else if (wParam == 'n' )
//            {
//                show_node_id = !show_node_id;
//                InvalidateRect (hWnd, NULL, TRUE);
//            }
//            // quit and write log file
//            else if (wParam == 'q' )
//            {
//                ShutSim ();
//                PostQuitMessage (0);
//            }
//            // move viewport UP
//            else if (wParam == 'w' )
//            {
//                g_origin.y += 5;
//                follow_mode = false;
//                InvalidateRect (hWnd, NULL, TRUE);
//            }
//            // move viewport DOWN
//            else if (wParam == 's' )
//            {
//                g_origin.y -= 5;
//                follow_mode = false;
//                InvalidateRect (hWnd, NULL, TRUE);
//            }
//            // move viewport LEFT
//            else if (wParam == 'a' )
//            {
//                g_origin.x += 5;
//                follow_mode = false;
//                InvalidateRect (hWnd, NULL, TRUE);
//            }
//            // move viewport RIGHT
//            else if (wParam == 'd' )
//            {
//                g_origin.x -= 5;
//                follow_mode = false;
//                InvalidateRect (hWnd, NULL, TRUE);
//            }
//            // reset viewport at 'origin'
//            else if (wParam == 'o' )
//            {
//                follow_mode = false;
//                g_origin.x = g_origin.y = 0;
//                InvalidateRect (hWnd, NULL, TRUE);
//            }
//            // display AOI and AOI neighbors
//            else if (wParam == 'i')
//            {
//                show_self = !show_self;
//                InvalidateRect (hWnd, NULL, TRUE);
//            }
//            // simply update status bar
//            else
//            {
//                RECT r;
//                r.top = r.left = 0;
//                r.right  = SCREEN_DIMENSION_X;
//                r.bottom = 100;
//                InvalidateRect (hWnd, &r, TRUE);
//            }
//            break;

//        case WM_MOUSEMOVE:
//            {
//                g_cursor = MAKEPOINTS (lParam);
//                RECT r;
//                r.top = r.left = 0;
//                r.right  = SCREEN_DIMENSION_X;
//                r.bottom = 100;
//                InvalidateRect (hWnd, &r, TRUE);
//            }
//            break;

//        // select current active
//        case WM_LBUTTONDOWN:
//            {
//                // select current active node
//                for (int i=0; i<g_nodes_created; i++)
//                {
//                    Node *n = GetNode (i);

//                    // skip failed nodes
//                    if (n == 0)
//                        continue;

//                    Position pt ((coord_t)g_cursor.x-g_origin.x, (coord_t)g_cursor.y-g_origin.y);

//                    if (n->aoi.center.distance (pt) <= NODE_RADIUS)
//                    {
//                        g_selected = i;

//                        // re-draw the whole screen
//                        InvalidateRect (hWnd, NULL, TRUE);
//                        break;
//                    }
//                }
//            }
//            break;

//        // toggle nodes showing their AOI
//        case WM_RBUTTONDOWN:
//            {
//                // select current active node
//                for (int i=0; i<g_nodes_created; i++)
//                {
//                    Node *n = GetNode(i);

//                    if (n == NULL)
//                        continue;

//                    Position pt ((coord_t)g_cursor.x-g_origin.x, (coord_t)g_cursor.y-g_origin.y);
//                    if (n->aoi.center.distance (pt) <= NODE_RADIUS)
//                    {
//                        g_show_aoi[i] = !g_show_aoi[i];
//                        InvalidateRect (hWnd, NULL, TRUE);
//                        break;
//                    }
//                }
//            }
//            break;

//        default:
//            return DefWindowProc (hWnd, msg, wParam, lParam);
//    }

//    return 0;
//}



//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
//INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
//{
//    VASTPara_Net netpara (VAST_NET_EMULATED);

//    // read parameters and initialize simulations
//    InitPara (VAST_NET_EMULATED, netpara, g_para);

//    InitSim (g_para, netpara);

//    // Register the window class
//    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
//                      hInst,
//                      LoadIcon(NULL,IDI_APPLICATION),
//                      LoadCursor(NULL,IDC_ARROW),
//                      (HBRUSH)(COLOR_WINDOW+1),
//                      NULL,
//                      "VASTSim", NULL };
//    RegisterClassEx (&wc);

//    // Create the application's window
//    HWND hWnd = CreateWindow ("VASTSim", "VAST Simulator (GUI) v 0.1",
//                              WS_OVERLAPPEDWINDOW, 100, 100, SCREEN_DIMENSION_X, SCREEN_DIMENSION_Y,
//                              GetDesktopWindow (), NULL, hInst, NULL);

//    // Show the window
//    ShowWindow (hWnd, SW_SHOWDEFAULT);
//    UpdateWindow (hWnd);

//    // set timer
//    SetTimer(hWnd,              // handle to main window
//        IDT_TIMER1,             // timer identifier
//        TIMER_INTERVAL,         // 1-second interval
//        (TIMERPROC) NULL);      // no timer callback

//    // set origin
//    g_origin.x = 0;
//    g_origin.y = 0;

//    // Enter the message loop
//    MSG msg;
//    ZeroMemory (&msg, sizeof (msg));

//    while (GetMessage (&msg, 0, 0, 0))
//    {
//        TranslateMessage (&msg);
//        DispatchMessage (&msg);
//    }

//    UnregisterClass ("VASTsim", hInst);

//    return msg.wParam;
//}


#endif // VASTVARIABLES_H
