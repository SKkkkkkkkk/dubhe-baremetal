#if defined A55
	#include "main_a55.c"
#elif defined RI5CY
	#include "main_ri5cy.c"
#elif defined CV32E40P
	#include "main_cv32e40p.c"
#elif defined(M3)
	#include "main_m3.c"
#else

#error "unknowed device!"

#endif