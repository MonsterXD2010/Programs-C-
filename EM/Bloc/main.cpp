#include <windows.h>
#include <dwmapi.h>
#include <commdlg.h>
#include <richedit.h>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <cstring>

#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "dwmapi.lib")

#define IDI_ICONO 101

// ============================================================
// GLOBALES
// ============================================================

HWND hEdit = NULL;
HWND hMarco = NULL;

HFONT fuenteEditor = NULL;

std::string archivoActual = "";
std::string rutaConfig;

enum TemaEditor
{
    TEMA_CLARO = 0,
    TEMA_OSCURO = 1
};

int temaActual = TEMA_CLARO;
int colorTextoIndice = 0;
int tamanoFuente = 16;
std::string nombreFuente = "Consolas";


// ============================================================
// COLORES CGA
// ============================================================

COLORREF coloresCGA[16] =
{
    RGB(0,   0,   0),
    RGB(0,   0,   170),
    RGB(0,   170, 0),
    RGB(0,   170, 170),
    RGB(170, 0,   0),
    RGB(170, 0,   170),
    RGB(170, 85,  0),
    RGB(170, 170, 170),
    RGB(85, 85,   85),
    RGB(85, 85,   255),
    RGB(85, 255,  85),
    RGB(85, 255,  255),
    RGB(255, 85,  85),
    RGB(255, 85,  255),
    RGB(255, 255,  85),
    RGB(255, 255, 255)
};


// ============================================================
// CONFIG
// ============================================================

std::string ObtenerRutaConfig()
{
    char ruta[MAX_PATH];

    GetModuleFileNameA(
        NULL,
        ruta,
        MAX_PATH
    );

    std::string resultado = ruta;

    size_t posicion =
        resultado.find_last_of("\\/");

    if (posicion != std::string::npos)
        resultado =
            resultado.substr(0, posicion + 1);

    resultado += "config.rm";

    return resultado;
}


// ------------------------------------------------------------

void CrearConfigDefault()
{
    std::ofstream archivo(rutaConfig);

    if (!archivo.is_open())
        return;

    archivo << "MODO=CLARO\n";
    archivo << "COLOR=0\n";
    archivo << "TAMANO=16\n";
    archivo << "FUENTE=Consolas\n";

    archivo.close();
}


// ------------------------------------------------------------

std::string QuitarEspacios(
    std::string texto
)
{
    while (!texto.empty() &&
          (texto.back() == ' ' ||
           texto.back() == '\r' ||
           texto.back() == '\n' ||
           texto.back() == '\t'))
    {
        texto.pop_back();
    }

    size_t inicio = 0;

    while (inicio < texto.size() &&
          (texto[inicio] == ' ' ||
           texto[inicio] == '\t'))
    {
        inicio++;
    }

    return texto.substr(inicio);
}


// ------------------------------------------------------------

void CargarConfig()
{
    std::ifstream archivo(rutaConfig);

    if (!archivo.is_open())
    {
        CrearConfigDefault();
        return;
    }

    std::string linea;

    while (std::getline(archivo, linea))
    {
        linea = QuitarEspacios(linea);

        size_t igual =
            linea.find('=');

        if (igual == std::string::npos)
            continue;

        std::string clave =
            QuitarEspacios(
                linea.substr(0, igual)
            );

        std::string valor =
            QuitarEspacios(
                linea.substr(igual + 1)
            );

        if (clave == "MODO")
        {
            if (valor == "OSCURO")
                temaActual = TEMA_OSCURO;
            else
                temaActual = TEMA_CLARO;
        }

        else if (clave == "COLOR")
        {
            int valorColor =
                atoi(valor.c_str());

            if (valorColor >= 0 &&
                valorColor < 16)
            {
                colorTextoIndice =
                    valorColor;
            }
        }

        else if (clave == "TAMANO")
        {
            int valorTamano =
                atoi(valor.c_str());

            if (valorTamano > 0 &&
                valorTamano < 200)
            {
                tamanoFuente =
                    valorTamano;
            }
        }

        else if (clave == "FUENTE")
        {
            if (!valor.empty())
                nombreFuente = valor;
        }
    }

    archivo.close();
}


// ------------------------------------------------------------

void GuardarConfig()
{
    std::ofstream archivo(rutaConfig);

    if (!archivo.is_open())
        return;

    if (temaActual == TEMA_OSCURO)
        archivo << "MODO=OSCURO\n";
    else
        archivo << "MODO=CLARO\n";

    archivo << "COLOR="
            << colorTextoIndice
            << "\n";

    archivo << "TAMANO="
            << tamanoFuente
            << "\n";

    archivo << "FUENTE="
            << nombreFuente
            << "\n";

    archivo.close();
}


// ============================================================
// COLORES DE INTERFAZ
// ============================================================

COLORREF ColorFondo()
{
    if (temaActual == TEMA_OSCURO)
        return RGB(30, 30, 30);

    return RGB(255, 255, 255);
}


COLORREF ColorMarco()
{
    if (temaActual == TEMA_OSCURO)
        return RGB(0, 0, 0);

    return RGB(180, 180, 180);
}


COLORREF ColorInterfaz()
{
    if (temaActual == TEMA_OSCURO)
        return RGB(35, 35, 35);

    return RGB(245, 245, 245);
}


COLORREF ColorTextoInterfaz()
{
    if (temaActual == TEMA_OSCURO)
        return RGB(240, 240, 240);

    return RGB(0, 0, 0);
}


// ============================================================
// BARRA DE TÍTULO OSCURA
// ============================================================

void ActualizarBarraTitulo(HWND hwnd)
{
    BOOL oscuro =
        (temaActual == TEMA_OSCURO);

    // Windows 10/11
    DwmSetWindowAttribute(
        hwnd,
        20,
        &oscuro,
        sizeof(oscuro)
    );
}


// ============================================================
// FUENTE
// ============================================================

void ActualizarFuente()
{
    if (fuenteEditor != NULL)
    {
        DeleteObject(fuenteEditor);
        fuenteEditor = NULL;
    }

    HDC hdc = GetDC(NULL);

    int dpi =
        GetDeviceCaps(
            hdc,
            LOGPIXELSY
        );

    ReleaseDC(NULL, hdc);

    fuenteEditor = CreateFontA(
        -MulDiv(
            tamanoFuente,
            dpi,
            72
        ),
        0,
        0,
        0,
        FW_NORMAL,
        FALSE,
        FALSE,
        FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        FIXED_PITCH | FF_MODERN,
        nombreFuente.c_str()
    );

    if (hEdit != NULL)
    {
        SendMessageA(
            hEdit,
            WM_SETFONT,
            (WPARAM)fuenteEditor,
            TRUE
        );
    }
}


// ============================================================
// COLOR DEL TEXTO
// ============================================================

void ActualizarColorTexto()
{
    if (hEdit == NULL)
        return;

    CHARRANGE rango;

    rango.cpMin = 0;
    rango.cpMax = -1;

    SendMessageA(
        hEdit,
        EM_EXSETSEL,
        0,
        (LPARAM)&rango
    );

    CHARFORMAT2A formato = {};

    formato.cbSize =
        sizeof(CHARFORMAT2A);

    formato.dwMask =
        CFM_COLOR;

    formato.crTextColor =
        coloresCGA[colorTextoIndice];

    SendMessageA(
        hEdit,
        EM_SETCHARFORMAT,
        SCF_SELECTION,
        (LPARAM)&formato
    );

    // Quitar selección
    rango.cpMin = 0;
    rango.cpMax = 0;

    SendMessageA(
        hEdit,
        EM_EXSETSEL,
        0,
        (LPARAM)&rango
    );
}


// ============================================================
// TEMA
// ============================================================

void ActualizarTema()
{
    if (hEdit != NULL)
    {
        SendMessageA(
            hEdit,
            EM_SETBKGNDCOLOR,
            0,
            ColorFondo()
        );

        InvalidateRect(
            hEdit,
            NULL,
            TRUE
        );
    }

    if (hMarco != NULL)
    {
        InvalidateRect(
            hMarco,
            NULL,
            TRUE
        );
    }
}


// ============================================================
// APARIENCIA COMPLETA
// ============================================================

void ActualizarApariencia(HWND hwnd)
{
    ActualizarFuente();
    ActualizarColorTexto();
    ActualizarTema();
    ActualizarBarraTitulo(hwnd);

    InvalidateRect(
        hwnd,
        NULL,
        TRUE
    );
}


// ============================================================
// MARCO
// ============================================================

LRESULT CALLBACK MarcoProc(
    HWND hwnd,
    UINT mensaje,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (mensaje)
    {
        case WM_ERASEBKGND:
        {
            HDC hdc = (HDC)wParam;

            RECT rect;

            GetClientRect(
                hwnd,
                &rect
            );

            HBRUSH pincel =
                CreateSolidBrush(
                    ColorMarco()
                );

            FillRect(
                hdc,
                &rect,
                pincel
            );

            DeleteObject(pincel);

            return 1;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;

            HDC hdc =
                BeginPaint(
                    hwnd,
                    &ps
                );

            RECT rect;

            GetClientRect(
                hwnd,
                &rect
            );

            HBRUSH pincel =
                CreateSolidBrush(
                    ColorMarco()
                );

            FillRect(
                hdc,
                &rect,
                pincel
            );

            DeleteObject(pincel);

            EndPaint(
                hwnd,
                &ps
            );

            return 0;
        }
    }

    return DefWindowProcA(
        hwnd,
        mensaje,
        wParam,
        lParam
    );
}


// ============================================================
// AUTOCOMPLETADO
// ============================================================

WNDPROC procedimientoOriginalEdit = NULL;

LRESULT CALLBACK EditProc(
    HWND hwnd,
    UINT mensaje,
    WPARAM wParam,
    LPARAM lParam
)
{
    if (mensaje == WM_CHAR)
    {
        char caracter =
            (char)wParam;

        char cerrar = 0;

        switch (caracter)
        {
            case '(':
                cerrar = ')';
                break;

            case '[':
                cerrar = ']';
                break;

            case '{':
                cerrar = '}';
                break;

            case '"':
                cerrar = '"';
                break;

            case '\'':
                cerrar = '\'';
                break;
        }

        if (cerrar != 0)
        {
            // Primero dejamos que se escriba
            // el carácter original.
            LRESULT resultado =
                CallWindowProcA(
                    procedimientoOriginalEdit,
                    hwnd,
                    mensaje,
                    wParam,
                    lParam
                );

            // Insertamos el segundo carácter.
            SendMessageA(
                hwnd,
                EM_REPLACESEL,
                TRUE,
                (LPARAM)&cerrar
            );

            // Obtener la posición donde quedó
            // el cursor después del primer carácter.
            CHARRANGE rango;

            SendMessageA(
                hwnd,
                EM_EXGETSEL,
                0,
                (LPARAM)&rango
            );

            // Retroceder uno:
            // ()|
            //   ↓
            // (|)
            rango.cpMin--;
            rango.cpMax--;

            SendMessageA(
                hwnd,
                EM_EXSETSEL,
                0,
                (LPARAM)&rango
            );

            return resultado;
        }
    }

    return CallWindowProcA(
        procedimientoOriginalEdit,
        hwnd,
        mensaje,
        wParam,
        lParam
    );
}


// ============================================================
// ARCHIVOS
// ============================================================

bool GuardarArchivoRuta(
    const std::string& ruta
)
{
    int longitud =
        GetWindowTextLengthA(hEdit);

    std::vector<char> buffer(
        longitud + 1
    );

    GetWindowTextA(
        hEdit,
        buffer.data(),
        longitud + 1
    );

    std::ofstream archivo(
        ruta,
        std::ios::binary
    );

    if (!archivo.is_open())
        return false;

    archivo.write(
        buffer.data(),
        longitud
    );

    archivo.close();

    return true;
}


// ------------------------------------------------------------

void AbrirArchivo(HWND hwnd)
{
    char nombreArchivo[MAX_PATH] = "";

    OPENFILENAMEA ofn = {};

    ofn.lStructSize =
        sizeof(ofn);

    ofn.hwndOwner = hwnd;

    ofn.lpstrFile =
        nombreArchivo;

    ofn.nMaxFile =
        MAX_PATH;

    ofn.lpstrFilter =
        "Todos los archivos\0*.*\0"
        "Archivos de texto\0*.txt\0";

    ofn.nFilterIndex = 1;

    ofn.Flags =
        OFN_PATHMUSTEXIST |
        OFN_FILEMUSTEXIST;

    if (!GetOpenFileNameA(&ofn))
        return;

    std::ifstream archivo(
        nombreArchivo,
        std::ios::binary
    );

    if (!archivo.is_open())
    {
        MessageBoxA(
            hwnd,
            "No se pudo abrir el archivo.",
            "Error",
            MB_ICONERROR
        );

        return;
    }

    std::string contenido(
        (std::istreambuf_iterator<char>(
            archivo
        )),
        std::istreambuf_iterator<char>()
    );

    archivo.close();

    SetWindowTextA(
        hEdit,
        contenido.c_str()
    );

    archivoActual =
        nombreArchivo;

    ActualizarColorTexto();

    SetWindowTextA(
        hwnd,
        ("Bloc - " +
         archivoActual).c_str()
    );
}


// ------------------------------------------------------------

void GuardarComo(HWND hwnd);

void GuardarArchivo(HWND hwnd)
{
    if (archivoActual.empty())
    {
        GuardarComo(hwnd);
        return;
    }

    if (!GuardarArchivoRuta(
        archivoActual
    ))
    {
        MessageBoxA(
            hwnd,
            "No se pudo guardar el archivo.",
            "Error",
            MB_ICONERROR
        );
    }
}


// ------------------------------------------------------------

void GuardarComo(HWND hwnd)
{
    char nombreArchivo[MAX_PATH] = "";

    OPENFILENAMEA ofn = {};

    ofn.lStructSize =
        sizeof(ofn);

    ofn.hwndOwner = hwnd;

    ofn.lpstrFile =
        nombreArchivo;

    ofn.nMaxFile =
        MAX_PATH;

    ofn.lpstrFilter =
        "Todos los archivos\0*.*\0"
        "Archivos de texto\0*.txt\0";

    ofn.nFilterIndex = 1;

    ofn.Flags =
        OFN_OVERWRITEPROMPT;

    if (!GetSaveFileNameA(&ofn))
        return;

    if (GuardarArchivoRuta(
        nombreArchivo
    ))
    {
        archivoActual =
            nombreArchivo;

        SetWindowTextA(
            hwnd,
            ("Bloc - " +
             archivoActual).c_str()
        );
    }
}


// ============================================================
// CONFIGURACIÓN
// ============================================================

HWND ventanaConfiguracion = NULL;

HWND cajaFuente = NULL;
HWND cajaTamano = NULL;
HWND comboTema = NULL;

int colorSeleccionado = 0;

HWND botonesColor[16];


// ============================================================
// ENUMERAR FUENTES DE WINDOWS
// ============================================================

std::vector<std::string> fuentesDisponibles;


int CALLBACK EnumerarFuentes(
    const LOGFONTA* logfont,
    const TEXTMETRICA* textmetric,
    DWORD tipo,
    LPARAM lParam
)
{
    std::string nombre =
        logfont->lfFaceName;

    if (nombre.empty())
        return 1;

    // Evitar duplicados
    for (const auto& fuente :
         fuentesDisponibles)
    {
        if (_stricmp(
                fuente.c_str(),
                nombre.c_str()
            ) == 0)
        {
            return 1;
        }
    }

    fuentesDisponibles.push_back(
        nombre
    );

    return 1;
}


// ------------------------------------------------------------

void CargarListaFuentes(HWND combo)
{
    fuentesDisponibles.clear();

    HDC hdc =
        GetDC(NULL);

    LOGFONTA logfont = {};

    logfont.lfCharSet =
        DEFAULT_CHARSET;

    EnumFontFamiliesExA(
        hdc,
        &logfont,
        (FONTENUMPROCA)
            EnumerarFuentes,
        0,
        0
    );

    ReleaseDC(NULL, hdc);

    for (const auto& fuente :
         fuentesDisponibles)
    {
        SendMessageA(
            combo,
            CB_ADDSTRING,
            0,
            (LPARAM)fuente.c_str()
        );
    }

    // Buscar la fuente actual
    for (int i = 0;
         i < (int)fuentesDisponibles.size();
         i++)
    {
        if (_stricmp(
                fuentesDisponibles[i].c_str(),
                nombreFuente.c_str()
            ) == 0)
        {
            SendMessageA(
                combo,
                CB_SETCURSEL,
                i,
                0
            );

            return;
        }
    }

    // Si no existe, mostrarla escrita
    SetWindowTextA(
        combo,
        nombreFuente.c_str()
    );
}


// ============================================================
// DIBUJAR BOTONES CGA
// ============================================================

void DibujarBotonColor(
    DRAWITEMSTRUCT* dis
)
{
    int indice =
        dis->CtlID - 300;

    if (indice < 0 ||
        indice >= 16)
        return;

    HDC hdc = dis->hDC;

    RECT rect =
        dis->rcItem;

    HBRUSH pincel =
        CreateSolidBrush(
            coloresCGA[indice]
        );

    FillRect(
        hdc,
        &rect,
        pincel
    );

    DeleteObject(pincel);

    HPEN lapiz;

    if (indice == colorSeleccionado)
    {
        lapiz =
            CreatePen(
                PS_SOLID,
                3,
                RGB(0, 120, 215)
            );
    }
    else
    {
        lapiz =
            CreatePen(
                PS_SOLID,
                1,
                RGB(80, 80, 80)
            );
    }

    HGDIOBJ anterior =
        SelectObject(
            hdc,
            lapiz
        );

    HGDIOBJ pincelAnterior =
        SelectObject(
            hdc,
            GetStockObject(
                NULL_BRUSH
            )
        );

    Rectangle(
        hdc,
        rect.left,
        rect.top,
        rect.right,
        rect.bottom
    );

    SelectObject(
        hdc,
        pincelAnterior
    );

    SelectObject(
        hdc,
        anterior
    );

    DeleteObject(lapiz);
}


// ============================================================
// CONFIGURACIÓN - PINTAR FONDO
// ============================================================

void PintarFondoConfig(
    HWND hwnd,
    HDC hdc
)
{
    RECT rect;

    GetClientRect(
        hwnd,
        &rect
    );

    HBRUSH pincel =
        CreateSolidBrush(
            ColorInterfaz()
        );

    FillRect(
        hdc,
        &rect,
        pincel
    );

    DeleteObject(pincel);
}


// ============================================================
// CONFIGURACIÓN
// ============================================================

LRESULT CALLBACK ConfigProc(
    HWND hwnd,
    UINT mensaje,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (mensaje)
    {
        case WM_CREATE:
        {
            CreateWindowA(
                "STATIC",
                "Fuente:",
                WS_CHILD | WS_VISIBLE,
                20, 15, 100, 20,
                hwnd,
                NULL,
                NULL,
                NULL
            );

            // Combo de fuentes
            cajaFuente =
                CreateWindowA(
                    "COMBOBOX",
                    "",
                    WS_CHILD |
                    WS_VISIBLE |
                    WS_VSCROLL |
                    CBS_DROPDOWN |
                    CBS_AUTOHSCROLL |
                    WS_BORDER,
                    20, 38, 230, 220,
                    hwnd,
                    (HMENU)201,
                    NULL,
                    NULL
                );

            CargarListaFuentes(
                cajaFuente
            );


            CreateWindowA(
                "STATIC",
                "Tama\xF1o:",
                WS_CHILD | WS_VISIBLE,
                270, 15, 100, 20,
                hwnd,
                NULL,
                NULL,
                NULL
            );

            char tamano[20];

            sprintf(
                tamano,
                "%d",
                tamanoFuente
            );

            cajaTamano =
                CreateWindowA(
                    "EDIT",
                    tamano,
                    WS_CHILD |
                    WS_VISIBLE |
                    WS_BORDER |
                    ES_NUMBER,
                    270, 38, 70, 25,
                    hwnd,
                    (HMENU)202,
                    NULL,
                    NULL
                );


            CreateWindowA(
                "STATIC",
                "Modo:",
                WS_CHILD | WS_VISIBLE,
                20, 78, 100, 20,
                hwnd,
                NULL,
                NULL,
                NULL
            );


            comboTema =
                CreateWindowA(
                    "COMBOBOX",
                    "",
                    WS_CHILD |
                    WS_VISIBLE |
                    CBS_DROPDOWNLIST |
                    WS_VSCROLL,
                    20, 101, 230, 120,
                    hwnd,
                    (HMENU)203,
                    NULL,
                    NULL
                );

            SendMessageA(
                comboTema,
                CB_ADDSTRING,
                0,
                (LPARAM)"Claro"
            );

            SendMessageA(
                comboTema,
                CB_ADDSTRING,
                0,
                (LPARAM)"Oscuro"
            );

            SendMessageA(
                comboTema,
                CB_SETCURSEL,
                temaActual,
                0
            );


            CreateWindowA(
                "STATIC",
                "Color del texto:",
                WS_CHILD | WS_VISIBLE,
                20, 140, 200, 20,
                hwnd,
                NULL,
                NULL,
                NULL
            );


            // 16 colores
            for (int i = 0;
                 i < 16;
                 i++)
            {
                int columna =
                    i % 8;

                int fila =
                    i / 8;

                int x =
                    20 + columna * 38;

                int y =
                    165 + fila * 38;

                botonesColor[i] =
                    CreateWindowA(
                        "BUTTON",
                        "",
                        WS_CHILD |
                        WS_VISIBLE |
                        BS_OWNERDRAW,
                        x,
                        y,
                        32,
                        32,
                        hwnd,
                        (HMENU)(INT_PTR)(300 + i),
                        NULL,
                        NULL
                    );
            }

            colorSeleccionado =
                colorTextoIndice;


            CreateWindowA(
                "BUTTON",
                "Aplicar",
                WS_CHILD |
                WS_VISIBLE |
                BS_DEFPUSHBUTTON,
                175, 250, 90, 30,
                hwnd,
                (HMENU)204,
                NULL,
                NULL
            );

            CreateWindowA(
                "BUTTON",
                "Cancelar",
                WS_CHILD |
                WS_VISIBLE,
                275, 250, 90, 30,
                hwnd,
                (HMENU)205,
                NULL,
                NULL
            );

            return 0;
        }


        case WM_CTLCOLORSTATIC:
        {
            HDC hdc =
                (HDC)wParam;

            SetTextColor(
                hdc,
                ColorTextoInterfaz()
            );

            SetBkColor(
                hdc,
                ColorInterfaz()
            );

            static HBRUSH pincel =
                NULL;

            if (pincel != NULL)
                DeleteObject(pincel);

            pincel =
                CreateSolidBrush(
                    ColorInterfaz()
                );

            return (LRESULT)pincel;
        }


        case WM_CTLCOLOREDIT:
        {
            HDC hdc =
                (HDC)wParam;

            SetTextColor(
                hdc,
                ColorTextoInterfaz()
            );

            SetBkColor(
                hdc,
                ColorInterfaz()
            );

            static HBRUSH pincel =
                NULL;

            if (pincel != NULL)
                DeleteObject(pincel);

            pincel =
                CreateSolidBrush(
                    ColorInterfaz()
                );

            return (LRESULT)pincel;
        }


        case WM_CTLCOLORLISTBOX:
        {
            HDC hdc =
                (HDC)wParam;

            SetTextColor(
                hdc,
                ColorTextoInterfaz()
            );

            SetBkColor(
                hdc,
                ColorInterfaz()
            );

            static HBRUSH pincel =
                NULL;

            if (pincel != NULL)
                DeleteObject(pincel);

            pincel =
                CreateSolidBrush(
                    ColorInterfaz()
                );

            return (LRESULT)pincel;
        }


        case WM_DRAWITEM:
        {
            DRAWITEMSTRUCT* dis =
                (DRAWITEMSTRUCT*)lParam;

            if (dis->CtlID >= 300 &&
                dis->CtlID < 316)
            {
                DibujarBotonColor(dis);
                return TRUE;
            }

            break;
        }


        case WM_COMMAND:
        {
            int id =
                LOWORD(wParam);

            if (id >= 300 &&
                id < 316)
            {
                colorSeleccionado =
                    id - 300;

                for (int i = 0;
                     i < 16;
                     i++)
                {
                    InvalidateRect(
                        botonesColor[i],
                        NULL,
                        TRUE
                    );
                }

                return 0;
            }


            if (id == 204)
            {
                char texto[256];


                // Fuente seleccionada
                int indice =
                    (int)SendMessageA(
                        cajaFuente,
                        CB_GETCURSEL,
                        0,
                        0
                    );

                if (indice != CB_ERR &&
                    indice <
                    (int)fuentesDisponibles.size())
                {
                    nombreFuente =
                        fuentesDisponibles[
                            indice
                        ];
                }


                // Tamaño
                GetWindowTextA(
                    cajaTamano,
                    texto,
                    sizeof(texto)
                );

                int nuevoTamano =
                    atoi(texto);

                if (nuevoTamano > 0 &&
                    nuevoTamano < 200)
                {
                    tamanoFuente =
                        nuevoTamano;
                }


                // Tema
                int tema =
                    (int)SendMessageA(
                        comboTema,
                        CB_GETCURSEL,
                        0,
                        0
                    );

                if (tema == 1)
                    temaActual =
                        TEMA_OSCURO;
                else
                    temaActual =
                        TEMA_CLARO;


                // Color
                colorTextoIndice =
                    colorSeleccionado;


                GuardarConfig();

                HWND principal =
                    GetWindow(
                        hwnd,
                        GW_OWNER
                    );

                ActualizarApariencia(
                    principal
                );


                DestroyWindow(hwnd);

                ventanaConfiguracion =
                    NULL;

                return 0;
            }


            if (id == 205)
            {
                DestroyWindow(hwnd);

                ventanaConfiguracion =
                    NULL;

                return 0;
            }

            break;
        }


        case WM_PAINT:
        {
            PAINTSTRUCT ps;

            HDC hdc =
                BeginPaint(
                    hwnd,
                    &ps
                );

            PintarFondoConfig(
                hwnd,
                hdc
            );

            EndPaint(
                hwnd,
                &ps
            );

            return 0;
        }


        case WM_CLOSE:
        {
            DestroyWindow(hwnd);

            ventanaConfiguracion =
                NULL;

            return 0;
        }
    }

    return DefWindowProcA(
        hwnd,
        mensaje,
        wParam,
        lParam
    );
}


// ============================================================
// ABRIR CONFIGURACIÓN
// ============================================================

void AbrirConfiguracion(
    HINSTANCE hInstance,
    HWND principal
)
{
    if (ventanaConfiguracion != NULL)
    {
        SetForegroundWindow(
            ventanaConfiguracion
        );

        return;
    }

    WNDCLASSA wc = {};

    wc.lpfnWndProc =
        ConfigProc;

    wc.hInstance =
        hInstance;

    wc.lpszClassName =
        "ConfiguracionBloc";

    wc.hCursor =
        LoadCursor(
            NULL,
            IDC_ARROW
        );

    wc.hbrBackground =
        (HBRUSH)(
            COLOR_WINDOW + 1
        );

    RegisterClassA(&wc);


    ventanaConfiguracion =
        CreateWindowExA(
            WS_EX_DLGMODALFRAME,
            "ConfiguracionBloc",
            "Configuracion",
            WS_OVERLAPPED |
            WS_CAPTION |
            WS_SYSMENU |
            WS_MINIMIZEBOX,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            390,
            330,
            principal,
            NULL,
            hInstance,
            NULL
        );


    if (ventanaConfiguracion == NULL)
        return;


    // Icono
    HICON icono =
        LoadIcon(
            hInstance,
            MAKEINTRESOURCE(
                IDI_ICONO
            )
        );

    SendMessage(
        ventanaConfiguracion,
        WM_SETICON,
        ICON_BIG,
        (LPARAM)icono
    );

    SendMessage(
        ventanaConfiguracion,
        WM_SETICON,
        ICON_SMALL,
        (LPARAM)icono
    );


    ActualizarBarraTitulo(
        ventanaConfiguracion
    );


    ShowWindow(
        ventanaConfiguracion,
        SW_SHOW
    );

    UpdateWindow(
        ventanaConfiguracion
    );
}


// ============================================================
// MENÚ
// ============================================================

#define ID_ABRIR        1001
#define ID_GUARDAR      1002
#define ID_GUARDARCOMO  1003
#define ID_SALIR        1004
#define ID_CONFIG       1005


HMENU CrearMenu()
{
    HMENU menuPrincipal =
        CreateMenu();

    HMENU menuArchivo =
        CreatePopupMenu();

    AppendMenuA(
        menuArchivo,
        MF_STRING,
        ID_ABRIR,
        "Abrir"
    );

    AppendMenuA(
        menuArchivo,
        MF_STRING,
        ID_GUARDAR,
        "Guardar"
    );

    AppendMenuA(
        menuArchivo,
        MF_STRING,
        ID_GUARDARCOMO,
        "Guardar como..."
    );

    AppendMenuA(
        menuArchivo,
        MF_SEPARATOR,
        0,
        NULL
    );

    AppendMenuA(
        menuArchivo,
        MF_STRING,
        ID_SALIR,
        "Salir"
    );


    HMENU menuConfiguracion =
        CreatePopupMenu();

    AppendMenuA(
        menuConfiguracion,
        MF_STRING,
        ID_CONFIG,
        "Configuracion"
    );


    AppendMenuA(
        menuPrincipal,
        MF_POPUP,
        (UINT_PTR)menuArchivo,
        "Archivo"
    );

    AppendMenuA(
        menuPrincipal,
        MF_POPUP,
        (UINT_PTR)menuConfiguracion,
        "Configuracion"
    );

    return menuPrincipal;
}


// ============================================================
// VENTANA PRINCIPAL
// ============================================================

LRESULT CALLBACK WndProc(
    HWND hwnd,
    UINT mensaje,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (mensaje)
    {
        case WM_CREATE:
        {
            HINSTANCE hInstance =
                (HINSTANCE)GetWindowLongPtrA(
                    hwnd,
                    GWLP_HINSTANCE
                );


            // Marco
            hMarco =
                CreateWindowA(
                    "MarcoEditor",
                    "",
                    WS_CHILD |
                    WS_VISIBLE,
                    0,
                    0,
                    0,
                    0,
                    hwnd,
                    NULL,
                    hInstance,
                    NULL
                );


            // RichEdit
            hEdit =
                CreateWindowExA(
                    0,
                    "RichEdit20A",
                    "",
                    WS_CHILD |
                    WS_VISIBLE |
                    WS_VSCROLL |
                    WS_HSCROLL |
                    ES_MULTILINE |
                    ES_AUTOVSCROLL |
                    ES_AUTOHSCROLL |
                    ES_NOOLEDRAGDROP,
                    2,
                    2,
                    0,
                    0,
                    hMarco,
                    NULL,
                    hInstance,
                    NULL
                );


            ActualizarApariencia(hwnd);


            int tab =
                16;

            SendMessageA(
                hEdit,
                EM_SETTABSTOPS,
                1,
                (LPARAM)&tab
            );


            procedimientoOriginalEdit =
                (WNDPROC)SetWindowLongPtrA(
                    hEdit,
                    GWLP_WNDPROC,
                    (LONG_PTR)EditProc
                );

            return 0;
        }


        case WM_SIZE:
        {
            int ancho =
                LOWORD(lParam);

            int alto =
                HIWORD(lParam);


            if (hMarco != NULL)
            {
                MoveWindow(
                    hMarco,
                    0,
                    0,
                    ancho,
                    alto,
                    TRUE
                );
            }


            if (hEdit != NULL)
            {
                MoveWindow(
                    hEdit,
                    2,
                    2,
                    ancho - 4,
                    alto - 4,
                    TRUE
                );
            }

            return 0;
        }


        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case ID_ABRIR:
                    AbrirArchivo(hwnd);
                    return 0;

                case ID_GUARDAR:
                    GuardarArchivo(hwnd);
                    return 0;

                case ID_GUARDARCOMO:
                    GuardarComo(hwnd);
                    return 0;

                case ID_CONFIG:
                {
                    HINSTANCE hInstance =
                        (HINSTANCE)GetWindowLongPtrA(
                            hwnd,
                            GWLP_HINSTANCE
                        );

                    AbrirConfiguracion(
                        hInstance,
                        hwnd
                    );

                    return 0;
                }

                case ID_SALIR:
                    DestroyWindow(hwnd);
                    return 0;
            }

            break;
        }


        case WM_DESTROY:
        {
            GuardarConfig();

            if (fuenteEditor != NULL)
            {
                DeleteObject(
                    fuenteEditor
                );

                fuenteEditor = NULL;
            }

            PostQuitMessage(0);

            return 0;
        }
    }

    return DefWindowProcA(
        hwnd,
        mensaje,
        wParam,
        lParam
    );
}


// ============================================================
// WINMAIN
// ============================================================

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
{
    rutaConfig =
        ObtenerRutaConfig();

    CargarConfig();


    // RichEdit
    LoadLibraryA(
        "Riched20.dll"
    );


    // Clase del marco
    WNDCLASSA wcMarco = {};

    wcMarco.lpfnWndProc =
        MarcoProc;

    wcMarco.hInstance =
        hInstance;

    wcMarco.lpszClassName =
        "MarcoEditor";

    wcMarco.hCursor =
        LoadCursor(
            NULL,
            IDC_ARROW
        );

    RegisterClassA(
        &wcMarco
    );


    // Clase principal
    WNDCLASSA wc = {};

    wc.lpfnWndProc =
        WndProc;

    wc.hInstance =
        hInstance;

    wc.lpszClassName =
        "BlocPrincipal";

    wc.hCursor =
        LoadCursor(
            NULL,
            IDC_ARROW
        );

    wc.hbrBackground =
        (HBRUSH)(
            COLOR_WINDOW + 1
        );

    // ICONO
    wc.hIcon =
        LoadIcon(
            hInstance,
            MAKEINTRESOURCE(
                IDI_ICONO
            )
        );


    if (!RegisterClassA(&wc))
        return 0;


    HWND hwnd =
        CreateWindowExA(
            0,
            "BlocPrincipal",
            "Bloc",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            900,
            650,
            NULL,
            NULL,
            hInstance,
            NULL
        );


    if (hwnd == NULL)
        return 0;


    // ICONO DE LA VENTANA
    HICON icono =
        LoadIcon(
            hInstance,
            MAKEINTRESOURCE(
                IDI_ICONO
            )
        );

    SendMessage(
        hwnd,
        WM_SETICON,
        ICON_BIG,
        (LPARAM)icono
    );

    SendMessage(
        hwnd,
        WM_SETICON,
        ICON_SMALL,
        (LPARAM)icono
    );


    SetMenu(
        hwnd,
        CrearMenu()
    );


    ActualizarBarraTitulo(
        hwnd
    );


    ShowWindow(
        hwnd,
        nCmdShow
    );

    UpdateWindow(hwnd);


    MSG mensaje;

    while (GetMessageA(
        &mensaje,
        NULL,
        0,
        0
    ))
    {
        TranslateMessage(
            &mensaje
        );

        DispatchMessageA(
            &mensaje
        );
    }


    return (int)mensaje.wParam;
}