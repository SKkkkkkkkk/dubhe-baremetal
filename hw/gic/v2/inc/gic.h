#if defined (A55) || (RI5CY)
#ifndef __GIC_H__
#define __GIC_H__

#include <stdint.h>
#include "gic_port.h"

#ifdef __cplusplus
 extern "C" {
#endif

/* IO definitions (access restrictions to peripheral registers) */
#ifdef __cplusplus
  #define   __I     volatile             /*!< \brief Defines 'read only' permissions */
#else
  #define   __I     volatile const       /*!< \brief Defines 'read only' permissions */
#endif
#define     __O     volatile             /*!< \brief Defines 'write only' permissions */
#define     __IO    volatile             /*!< \brief Defines 'read / write' permissions */

/* following defines should be used for structure members */
#define     __IM     volatile const      /*!< \brief Defines 'read only' structure member permissions */
#define     __OM     volatile            /*!< \brief Defines 'write only' structure member permissions */
#define     __IOM    volatile            /*!< \brief Defines 'read / write' structure member permissions */
#define RESERVED(N, T) T RESERVED##N;    // placeholder struct members used for "reserved" areas

#define __STATIC_INLINE                        static inline
    
/** \brief  Structure type to access the Generic Interrupt Controller Distributor (GICD)
*/
typedef struct
{
  __IOM uint32_t CTLR;                 /*!< \brief  Offset: 0x000 (R/W) Distributor Control Register */
  __IM  uint32_t TYPER;                /*!< \brief  Offset: 0x004 (R/ ) Interrupt Controller Type Register */
  __IM  uint32_t IIDR;                 /*!< \brief  Offset: 0x008 (R/ ) Distributor Implementer Identification Register */
        RESERVED(0, uint32_t)
  __IOM uint32_t STATUSR;              /*!< \brief  Offset: 0x010 (R/W) Error Reporting Status Register, optional */
        RESERVED(1[11], uint32_t)
  __OM  uint32_t SETSPI_NSR;           /*!< \brief  Offset: 0x040 ( /W) Set SPI Register */
        RESERVED(2, uint32_t)
  __OM  uint32_t CLRSPI_NSR;           /*!< \brief  Offset: 0x048 ( /W) Clear SPI Register */
        RESERVED(3, uint32_t)
  __OM  uint32_t SETSPI_SR;            /*!< \brief  Offset: 0x050 ( /W) Set SPI, Secure Register */
        RESERVED(4, uint32_t)
  __OM  uint32_t CLRSPI_SR;            /*!< \brief  Offset: 0x058 ( /W) Clear SPI, Secure Register */
        RESERVED(5[9], uint32_t)
  __IOM uint32_t IGROUPR[32];          /*!< \brief  Offset: 0x080 (R/W) Interrupt Group Registers */
  __IOM uint32_t ISENABLER[32];        /*!< \brief  Offset: 0x100 (R/W) Interrupt Set-Enable Registers */
  __IOM uint32_t ICENABLER[32];        /*!< \brief  Offset: 0x180 (R/W) Interrupt Clear-Enable Registers */
  __IOM uint32_t ISPENDR[32];          /*!< \brief  Offset: 0x200 (R/W) Interrupt Set-Pending Registers */
  __IOM uint32_t ICPENDR[32];          /*!< \brief  Offset: 0x280 (R/W) Interrupt Clear-Pending Registers */
  __IOM uint32_t ISACTIVER[32];        /*!< \brief  Offset: 0x300 (R/W) Interrupt Set-Active Registers */
  __IOM uint32_t ICACTIVER[32];        /*!< \brief  Offset: 0x380 (R/W) Interrupt Clear-Active Registers */
  __IOM uint32_t IPRIORITYR[255];      /*!< \brief  Offset: 0x400 (R/W) Interrupt Priority Registers */
        RESERVED(6, uint32_t)
  __IOM uint32_t  ITARGETSR[255];      /*!< \brief  Offset: 0x800 (R/W) Interrupt Targets Registers */
        RESERVED(7, uint32_t)
  __IOM uint32_t ICFGR[64];            /*!< \brief  Offset: 0xC00 (R/W) Interrupt Configuration Registers */
  __IOM uint32_t IGRPMODR[32];         /*!< \brief  Offset: 0xD00 (R/W) Interrupt Group Modifier Registers */
        RESERVED(8[32], uint32_t)
  __IOM uint32_t NSACR[64];            /*!< \brief  Offset: 0xE00 (R/W) Non-secure Access Control Registers */
  __OM  uint32_t SGIR;                 /*!< \brief  Offset: 0xF00 ( /W) Software Generated Interrupt Register */
        RESERVED(9[3], uint32_t)
  __IOM uint32_t CPENDSGIR[4];         /*!< \brief  Offset: 0xF10 (R/W) SGI Clear-Pending Registers */
  __IOM uint32_t SPENDSGIR[4];         /*!< \brief  Offset: 0xF20 (R/W) SGI Set-Pending Registers */
        RESERVED(10[5236], uint32_t)
  __IOM uint64_t IROUTER[988];         /*!< \brief  Offset: 0x6100(R/W) Interrupt Routing Registers */
}  GICDistributor_Type;

#define GICDistributor      ((GICDistributor_Type      *)     GIC_DISTRIBUTOR_BASE ) /*!< \brief GIC Distributor register set access pointer */

/** \brief  Structure type to access the Generic Interrupt Controller Interface (GICC)
*/
typedef struct
{
  __IOM uint32_t CTLR;                 /*!< \brief  Offset: 0x000 (R/W) CPU Interface Control Register */
  __IOM uint32_t PMR;                  /*!< \brief  Offset: 0x004 (R/W) Interrupt Priority Mask Register */
  __IOM uint32_t BPR;                  /*!< \brief  Offset: 0x008 (R/W) Binary Point Register */
  __IM  uint32_t IAR;                  /*!< \brief  Offset: 0x00C (R/ ) Interrupt Acknowledge Register */
  __OM  uint32_t EOIR;                 /*!< \brief  Offset: 0x010 ( /W) End Of Interrupt Register */
  __IM  uint32_t RPR;                  /*!< \brief  Offset: 0x014 (R/ ) Running Priority Register */
  __IM  uint32_t HPPIR;                /*!< \brief  Offset: 0x018 (R/ ) Highest Priority Pending Interrupt Register */
  __IOM uint32_t ABPR;                 /*!< \brief  Offset: 0x01C (R/W) Aliased Binary Point Register */
  __IM  uint32_t AIAR;                 /*!< \brief  Offset: 0x020 (R/ ) Aliased Interrupt Acknowledge Register */
  __OM  uint32_t AEOIR;                /*!< \brief  Offset: 0x024 ( /W) Aliased End Of Interrupt Register */
  __IM  uint32_t AHPPIR;               /*!< \brief  Offset: 0x028 (R/ ) Aliased Highest Priority Pending Interrupt Register */
  __IOM uint32_t STATUSR;              /*!< \brief  Offset: 0x02C (R/W) Error Reporting Status Register, optional */
        RESERVED(1[40], uint32_t)
  __IOM uint32_t APR[4];               /*!< \brief  Offset: 0x0D0 (R/W) Active Priority Register */
  __IOM uint32_t NSAPR[4];             /*!< \brief  Offset: 0x0E0 (R/W) Non-secure Active Priority Register */
        RESERVED(2[3], uint32_t)
  __IM  uint32_t IIDR;                 /*!< \brief  Offset: 0x0FC (R/ ) CPU Interface Identification Register */
        RESERVED(3[960], uint32_t)
  __OM  uint32_t DIR;                  /*!< \brief  Offset: 0x1000( /W) Deactivate Interrupt Register */
}  GICInterface_Type;

#define GICInterface        ((GICInterface_Type        *)     GIC_INTERFACE_BASE )   /*!< \brief GIC Interface register set access pointer */


/* ##########################  GIC functions  ###################################### */
/** \brief  Enable the interrupt distributor using the GIC's CTLR register.
*/
__STATIC_INLINE void GIC_EnableDistributor(void)
{
  GICDistributor->CTLR |= 1U;
}

/** \brief Disable the interrupt distributor using the GIC's CTLR register.
*/
__STATIC_INLINE void GIC_DisableDistributor(void)
{
  GICDistributor->CTLR &=~1U;
}

/** \brief Read the GIC's TYPER register.
* \return GICDistributor_Type::TYPER
*/
__STATIC_INLINE uint32_t GIC_DistributorInfo(void)
{
  return (GICDistributor->TYPER);
}

/** \brief Reads the GIC's IIDR register.
* \return GICDistributor_Type::IIDR
*/
__STATIC_INLINE uint32_t GIC_DistributorImplementer(void)
{
  return (GICDistributor->IIDR);
}

/** \brief Sets the GIC's ITARGETSR register for the given interrupt.
* \param [in] IRQn Interrupt to be configured.
* \param [in] cpu_target CPU interfaces to assign this interrupt to.
*/
__STATIC_INLINE void GIC_SetTarget(IRQn_Type IRQn, uint32_t cpu_target)
{
  uint32_t mask = GICDistributor->ITARGETSR[IRQn / 4U] & ~(0xFFUL << ((IRQn % 4U) * 8U));
  GICDistributor->ITARGETSR[IRQn / 4U] = mask | ((cpu_target & 0xFFUL) << ((IRQn % 4U) * 8U));
}

/** \brief Read the GIC's ITARGETSR register.
* \param [in] IRQn Interrupt to acquire the configuration for.
* \return GICDistributor_Type::ITARGETSR
*/
__STATIC_INLINE uint32_t GIC_GetTarget(IRQn_Type IRQn)
{
  return (GICDistributor->ITARGETSR[IRQn / 4U] >> ((IRQn % 4U) * 8U)) & 0xFFUL;
}

/** \brief Enable the CPU's interrupt interface.
*/
__STATIC_INLINE void GIC_EnableInterface(void)
{
  GICInterface->CTLR |= 1U; //enable interface
}

/** \brief Disable the CPU's interrupt interface.
*/
__STATIC_INLINE void GIC_DisableInterface(void)
{
  GICInterface->CTLR &=~1U; //disable distributor
}

/** \brief Read the CPU's IAR register.
* \return GICInterface_Type::IAR
*/
__STATIC_INLINE IRQn_Type GIC_AcknowledgePending(void)
{
  return (IRQn_Type)(GICInterface->IAR);
}

/** \brief Writes the given interrupt number to the CPU's EOIR register.
* \param [in] IRQn The interrupt to be signaled as finished.
*/
__STATIC_INLINE void GIC_EndInterrupt(IRQn_Type IRQn)
{
  GICInterface->EOIR = IRQn;
}

/** \brief Enables the given interrupt using GIC's ISENABLER register.
* \param [in] IRQn The interrupt to be enabled.
*/
__STATIC_INLINE void GIC_EnableIRQ(IRQn_Type IRQn)
{
  GICDistributor->ISENABLER[IRQn / 32U] = 1U << (IRQn % 32U);
}

/** \brief Get interrupt enable status using GIC's ISENABLER register.
* \param [in] IRQn The interrupt to be queried.
* \return 0 - interrupt is not enabled, 1 - interrupt is enabled.
*/
__STATIC_INLINE uint32_t GIC_GetEnableIRQ(IRQn_Type IRQn)
{
  return (GICDistributor->ISENABLER[IRQn / 32U] >> (IRQn % 32U)) & 1UL;
}

/** \brief Disables the given interrupt using GIC's ICENABLER register.
* \param [in] IRQn The interrupt to be disabled.
*/
__STATIC_INLINE void GIC_DisableIRQ(IRQn_Type IRQn)
{
  GICDistributor->ICENABLER[IRQn / 32U] = 1U << (IRQn % 32U);
}

/** \brief Get interrupt pending status from GIC's ISPENDR register.
* \param [in] IRQn The interrupt to be queried.
* \return 0 - interrupt is not pending, 1 - interrupt is pendig.
*/
__STATIC_INLINE uint32_t GIC_GetPendingIRQ(IRQn_Type IRQn)
{
  uint32_t pend;

  if (IRQn >= 16U) {
    pend = (GICDistributor->ISPENDR[IRQn / 32U] >> (IRQn % 32U)) & 1UL;
  } else {
    // INTID 0-15 Software Generated Interrupt
    pend = (GICDistributor->SPENDSGIR[IRQn / 4U] >> ((IRQn % 4U) * 8U)) & 0xFFUL;
    // No CPU identification offered
    if (pend != 0U) {
      pend = 1U;
    } else {
      pend = 0U;
    }
  }

  return (pend);
}

/** \brief Sets the given interrupt as pending using GIC's ISPENDR register.
* \param [in] IRQn The interrupt to be enabled.
*/
__STATIC_INLINE void GIC_SetPendingIRQ(IRQn_Type IRQn)
{
  if (IRQn >= 16U) {
    GICDistributor->ISPENDR[IRQn / 32U] = 1U << (IRQn % 32U);
  } else {
    // INTID 0-15 Software Generated Interrupt
    // Forward the interrupt to the CPU interface that requested it
    GICDistributor->SGIR = (IRQn | 0x02000000U);
  }
}

/** \brief Clears the given interrupt from being pending using GIC's ICPENDR register.
* \param [in] IRQn The interrupt to be enabled.
*/
__STATIC_INLINE void GIC_ClearPendingIRQ(IRQn_Type IRQn)
{
  if (IRQn >= 16U) {
    GICDistributor->ICPENDR[IRQn / 32U] = 1U << (IRQn % 32U);
  } else {
    // INTID 0-15 Software Generated Interrupt
    GICDistributor->CPENDSGIR[IRQn / 4U] = 1U << ((IRQn % 4U) * 8U);
  }
}

/** \brief Sets the interrupt configuration using GIC's ICFGR register.
* \param [in] IRQn The interrupt to be configured.
* \param [in] int_config Int_config field value. Bit 0: Reserved (0 - N-N model, 1 - 1-N model for some GIC before v1)
*                                           Bit 1: 0 - level sensitive, 1 - edge triggered
*/
__STATIC_INLINE void GIC_SetConfiguration(IRQn_Type IRQn, uint32_t int_config)
{
  uint32_t icfgr = GICDistributor->ICFGR[IRQn / 16U];
  uint32_t shift = (IRQn % 16U) << 1U;

  icfgr &= (~(3U         << shift));
  icfgr |= (  int_config << shift);

  GICDistributor->ICFGR[IRQn / 16U] = icfgr;
}

/** \brief Get the interrupt configuration from the GIC's ICFGR register.
* \param [in] IRQn Interrupt to acquire the configuration for.
* \return Int_config field value. Bit 0: Reserved (0 - N-N model, 1 - 1-N model for some GIC before v1)
*                                 Bit 1: 0 - level sensitive, 1 - edge triggered
*/
__STATIC_INLINE uint32_t GIC_GetConfiguration(IRQn_Type IRQn)
{
  return (GICDistributor->ICFGR[IRQn / 16U] >> ((IRQn % 16U) >> 1U));
}

/** \brief Set the priority for the given interrupt in the GIC's IPRIORITYR register.
* \param [in] IRQn The interrupt to be configured.
* \param [in] priority The priority for the interrupt, lower values denote higher priorities.
*/
__STATIC_INLINE void GIC_SetPriority(IRQn_Type IRQn, uint32_t priority)
{
  uint32_t mask = GICDistributor->IPRIORITYR[IRQn / 4U] & ~(0xFFUL << ((IRQn % 4U) * 8U));
  GICDistributor->IPRIORITYR[IRQn / 4U] = mask | ((priority & 0xFFUL) << ((IRQn % 4U) * 8U));
}

/** \brief Read the current interrupt priority from GIC's IPRIORITYR register.
* \param [in] IRQn The interrupt to be queried.
*/
__STATIC_INLINE uint32_t GIC_GetPriority(IRQn_Type IRQn)
{
  return (GICDistributor->IPRIORITYR[IRQn / 4U] >> ((IRQn % 4U) * 8U)) & 0xFFUL;
}

/** \brief Set the interrupt priority mask using CPU's PMR register.
* \param [in] priority Priority mask to be set.
*/
__STATIC_INLINE void GIC_SetInterfacePriorityMask(uint32_t priority)
{
  GICInterface->PMR = priority & 0xFFUL; //set priority mask
}

/** \brief Read the current interrupt priority mask from CPU's PMR register.
* \result GICInterface_Type::PMR
*/
__STATIC_INLINE uint32_t GIC_GetInterfacePriorityMask(void)
{
  return GICInterface->PMR;
}

/** \brief Configures the group priority and subpriority split point using CPU's BPR register.
* \param [in] binary_point Amount of bits used as subpriority.
*/
__STATIC_INLINE void GIC_SetBinaryPoint(uint32_t binary_point)
{
  GICInterface->BPR = binary_point & 7U; //set binary point
}

/** \brief Read the current group priority and subpriority split point from CPU's BPR register.
* \return GICInterface_Type::BPR
*/
__STATIC_INLINE uint32_t GIC_GetBinaryPoint(void)
{
  return GICInterface->BPR;
}

/** \brief Get the status for a given interrupt.
* \param [in] IRQn The interrupt to get status for.
* \return 0 - not pending/active, 1 - pending, 2 - active, 3 - pending and active
*/
__STATIC_INLINE uint32_t GIC_GetIRQStatus(IRQn_Type IRQn)
{
  uint32_t pending, active;

  active = ((GICDistributor->ISACTIVER[IRQn / 32U])  >> (IRQn % 32U)) & 1UL;
  pending = ((GICDistributor->ISPENDR[IRQn / 32U]) >> (IRQn % 32U)) & 1UL;

  return ((active<<1U) | pending);
}

/** \brief Generate a software interrupt using GIC's SGIR register.
* \param [in] IRQn Software interrupt to be generated.
* \param [in] target_list List of CPUs the software interrupt should be forwarded to.
* \param [in] filter_list Filter to be applied to determine interrupt receivers.
*/
__STATIC_INLINE void GIC_SendSGI(IRQn_Type IRQn, uint32_t target_list, uint32_t filter_list)
{
  GICDistributor->SGIR = ((filter_list & 3U) << 24U) | ((target_list & 0xFFUL) << 16U) | (IRQn & 0x0FUL);
}

/** \brief Get the interrupt number of the highest interrupt pending from CPU's HPPIR register.
* \return GICInterface_Type::HPPIR
*/
__STATIC_INLINE uint32_t GIC_GetHighPendingIRQ(void) 
{ 
  return GICInterface->HPPIR; 
}

/** \brief Provides information about the implementer and revision of the CPU interface.
* \return GICInterface_Type::IIDR
*/
__STATIC_INLINE uint32_t GIC_GetInterfaceId(void)
{ 
  return GICInterface->IIDR; 
}

/** \brief Set the interrupt group from the GIC's IGROUPR register.
* \param [in] IRQn The interrupt to be queried.
* \param [in] group Interrupt group number: 0 - Group 0, 1 - Group 1
*/
__STATIC_INLINE void GIC_SetGroup(IRQn_Type IRQn, uint32_t group)
{
  uint32_t igroupr = GICDistributor->IGROUPR[IRQn / 32U];
  uint32_t shift   = (IRQn % 32U);

  igroupr &= (~(1U          << shift));
  igroupr |= ( (group & 1U) << shift);

  GICDistributor->IGROUPR[IRQn / 32U] = igroupr;
}
#define GIC_SetSecurity         GIC_SetGroup

/** \brief Get the interrupt group from the GIC's IGROUPR register.
* \param [in] IRQn The interrupt to be queried.
* \return 0 - Group 0, 1 - Group 1
*/
__STATIC_INLINE uint32_t GIC_GetGroup(IRQn_Type IRQn)
{
  return (GICDistributor->IGROUPR[IRQn / 32U] >> (IRQn % 32U)) & 1UL;
}
#define GIC_GetSecurity         GIC_GetGroup

/** \brief Initialize the interrupt distributor.
*/
__STATIC_INLINE void GIC_DistInit(void)
{
  uint32_t i;
  uint32_t num_irq = 0U;
  uint32_t priority_field;

  //A reset sets all bits in the IGROUPRs corresponding to the SPIs to 0,
  //configuring all of the interrupts as Secure.

  //Disable interrupt forwarding
  GIC_DisableDistributor();
  //Get the maximum number of interrupts that the GIC supports
  num_irq = 32U * ((GIC_DistributorInfo() & 0x1FU) + 1U);

  /* Priority level is implementation defined.
   To determine the number of priority bits implemented write 0xFF to an IPRIORITYR
   priority field and read back the value stored.*/
  GIC_SetPriority((IRQn_Type)0U, 0xFFU);
  priority_field = GIC_GetPriority((IRQn_Type)0U);

  for (i = 32U; i < num_irq; i++)
  {
      //Disable the SPI interrupt
      GIC_DisableIRQ((IRQn_Type)i);
      //Set level-sensitive (and N-N model)
      GIC_SetConfiguration((IRQn_Type)i, 0U);
      //Set priority
      GIC_SetPriority((IRQn_Type)i, priority_field/2);
      //Set target list to CPU0
      GIC_SetTarget((IRQn_Type)i, 1U);
  }
  //Enable distributor
  GIC_EnableDistributor();
}

/** \brief Initialize the CPU's interrupt interface
*/
__STATIC_INLINE void GIC_CPUInterfaceInit(void)
{
  uint32_t i;
  uint32_t priority_field;

  //A reset sets all bits in the IGROUPRs corresponding to the SPIs to 0,
  //configuring all of the interrupts as Secure.

  //Disable interrupt forwarding
  GIC_DisableInterface();

  /* Priority level is implementation defined.
   To determine the number of priority bits implemented write 0xFF to an IPRIORITYR
   priority field and read back the value stored.*/
  GIC_SetPriority((IRQn_Type)0U, 0xFFU);
  priority_field = GIC_GetPriority((IRQn_Type)0U);

  //SGI and PPI
  for (i = 0U; i < 32U; i++)
  {
    if(i > 15U) {
      //Set level-sensitive (and N-N model) for PPI
      GIC_SetConfiguration((IRQn_Type)i, 0U);
    }
    //Disable SGI and PPI interrupts
    GIC_DisableIRQ((IRQn_Type)i);
    //Set priority
    GIC_SetPriority((IRQn_Type)i, priority_field/2);
  }
  //Enable interface
  GIC_EnableInterface();
  //Set binary point
  // GIC_SetBinaryPoint(7U); //禁止中断嵌套
  GIC_SetBinaryPoint(2U); //允许嵌套
  //Set priority mask
  GIC_SetInterfacePriorityMask(0xFFU);
}

/** \brief Initialize and enable the GIC
*/
__STATIC_INLINE void GIC_Enable(void)
{
  GIC_DistInit();
  GIC_CPUInterfaceInit(); //per CPU
}


#endif

#endif