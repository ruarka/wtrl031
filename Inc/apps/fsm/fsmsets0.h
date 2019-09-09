/*! \file      fsmsets0.h
    \version   0.1
    \date      2017-06-06 23:45
    \brief     <A Documented file.>
    \details   <Details.>
    \bug       
    \copyright  
    \author    
*/
/* ------------------------------------------------------------------------------------------------
 *                                     Macros
 * ------------------------------------------------------------------------------------------------
 */

/* ------------------------------------------------------------------------------------------------
 *                                 Type definitions
 * ------------------------------------------------------------------------------------------------
 */

/** \var pfnMenuMake
    \brief Defines a callback function for "MakeFunc" typed menu leaf

    After menu leaf "MakeFunc" is chosen defined callback function is called before
    parent leaf become actual.
    if NULL then nothing is happened.
*/
typedef  void (*pfnMenuMake)( void );

/** \var pfnMenuBeforeLeaf
    \brief Defines a callback function called before selected leaf becomes active.

    After menu leaf is selected pfnMenuBeforeLeaf function corresponded to selected
    leaf is called.
    if NULL then nothing is happened.
*/
typedef  void (*pfnMenuBeforeLeaf)( void );

/** \var pfnMenuCustomPrint
    \brief Defines a callback for printing display customization

    if NULL then nothing is happened.
*/
typedef  void (*pfnMenuCustomPrint)( uint8_t row, char* pSBuff );

/**
 * @brief  Menu Leaf description object
 */
typedef const struct _menu_def
{
  const struct _menu_def  *Next;              /*!< Next menu leaf - Horizontal navigation         */
  const struct _menu_def  *Previous;          /*!< Previous menu leaf - Horizontal navigation     */
  const struct _menu_def  *Parent;            /*!< Previous menu leaf - Vertical navigation       */
  const struct _menu_def  *Child;             /*!< Next menu leaf - Vertical navigation           */
  const void*             pReturnVal;         /*!< Pointer to result variable                     */
  const void*             fnMenuBeforeLeaf;   /*!< Func called before selected leaf               */
  const void*             fnMenuCustomPrint;  /*!< Func to customize display printing             */
  const uint32_t          minVal;             /*!< Min integer result value                       */
  const uint32_t          maxVal;             /*!< Max integer result value                       */
  tFsmMenueType           subMenuType;        /*!< Menu leaf type used to differ menu actions     */
  const char              Name[21];           /*!< Menu leaf description used for display         */
} tMenu, *tPMenu;
 /* ------------------------------------------------------------------------------------------------
 *                                    Functions declarations
 * ------------------------------------------------------------------------------------------------
 */
extern tMenu* curr;

extern tFsmMenuIfs menuSets0State;
