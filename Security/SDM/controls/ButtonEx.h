#pragma once
#include "ImageEx.h"

//��ť��״̬
enum
{
    CTRL_NOFOCUS = 0x01,            //��ͨ
    CTRL_FOCUS,                     //mousemove
    CTRL_SELECTED,                  //buttondown
    CTRL_DISABLE,                   //��Ч
};

//ͼƬ��ʽ
enum
{
    BTN_IMG_1 = 1,                  //
    BTN_IMG_3 = 3,                  //3��ͼ��1��ͼƬ����3Сͼ����ͬ��
    BTN_IMG_4 = 4,                  //4��ͼ
};

//��ť����
enum
{
    BTN_TYPE_NORMAL = 0x10,         //��ͨBTN
    BTN_TYPE_MENU,                  //�˵����͵�BTN
    BTN_TYPE_STATIC,                //��̬���͵�BTN
};

class CButtonEx :
    public CButton
{
        DECLARE_DYNAMIC(CButtonEx)
        
    public:
        CButtonEx(void);
        virtual ~CButtonEx(void);
        
        bool FixButtonSize();
        bool SetTextColor(COLORREF crTextColor);
        bool SetButtonImage(LPCTSTR pszFileName, bool bExpandImage = false);
        bool SetButtonImage(HINSTANCE hInstance, LPCTSTR pszResourceName, bool bExpandImage = false);
        void PaintParent();
        
        afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
        
    protected:
        virtual void PreSubclassWindow();
        virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
        
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
        afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
        afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam);
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        
        bool m_bExpand;
        bool m_bHovering;
        COLORREF m_crTextColor;
        CImageEx m_ImageBack;
        
        UINT m_nState;
        UINT m_nBtnType;
        BOOL m_bMenuOn;     //BTN����ΪBTN_TYPE_MENUʱ���Ƿ��ڰ��µ�״̬
        
        DECLARE_MESSAGE_MAP()
};

