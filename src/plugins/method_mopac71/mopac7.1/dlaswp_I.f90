      MODULE dlaswp_I   
      INTERFACE
!...Generated by Pacific-Sierra Research 77to90  4.4G  10:47:04  03/09/06  
      SUBROUTINE dlaswp (N, A, LDA, K1, K2, IPIV, INCX) 
      USE vast_kind_param,ONLY: DOUBLE 
      integer :: N       
      integer, INTENT(IN) :: LDA 
      integer, INTENT(IN) :: K1 
      integer, INTENT(IN) :: K2 
      integer, DIMENSION(*), INTENT(IN) :: IPIV 
      integer, INTENT(IN) :: INCX 
      real(DOUBLE), DIMENSION(LDA,*) :: A 
      END SUBROUTINE  
      END INTERFACE 
      END MODULE 
