      MODULE prthes_I   
      INTERFACE
!...Generated by Pacific-Sierra Research 77to90  4.4G  12:12:16  03/10/06  
      SUBROUTINE prthes (EIGVAL, NVAR, HESS, U) 
      USE vast_kind_param,ONLY: DOUBLE 
      integer, INTENT(IN) :: NVAR
      real(DOUBLE), DIMENSION(nvar), INTENT(IN) :: EIGVAL        
      real(DOUBLE), DIMENSION(NVAR,NVAR), INTENT(IN) :: HESS 
      real(DOUBLE), DIMENSION(NVAR,NVAR), INTENT(IN) :: U 
      END SUBROUTINE  
      END INTERFACE 
      END MODULE 