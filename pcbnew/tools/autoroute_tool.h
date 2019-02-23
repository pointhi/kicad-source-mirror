#ifndef __AUTOROUTE_TOOL_H
#define __AUTOROUTE_TOOL_H


#include "pcb_tool.h"

struct pcb;

class AUTOROUTE_TOOL : public PCB_TOOL
{
public:
    AUTOROUTE_TOOL();
    ~AUTOROUTE_TOOL();

    ///> React to model/view changes
    void Reset( RESET_REASON aReason ) override;

    ///> Basic initalization
    bool Init() override;

    ///> Bind handlers to corresponding TOOL_ACTIONs
    void setTransitions() override;

private:

    void outputBoard( pcb& aPcb, BOARD_COMMIT& commit, int x_offset, int y_offset );

    ///> 'Route All' tool
    int routeAll( const TOOL_EVENT& aEvent );

    ///> 'Route Track' tool
    int routeTrack( const TOOL_EVENT& aEvent );

    PCB_EDIT_FRAME* m_frame;
    BOARD* m_board;
};


#endif //__AUTOROUTE_TOOL_H
