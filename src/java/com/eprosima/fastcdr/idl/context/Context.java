package com.eprosima.fastcdr.idl.context;

public interface Context
{
    /*** Functions to select what to print in string templates ***/

    /*!
     * @brief This function is used to stringtemplates to know if the exception type
     * has to be printed.
     */
    public boolean isPrintexception();

    /*!
     * @brief This function is used to stringtemplates to know if the operation type
     * has to be printed.
     */
    public boolean isPrintoperation();

    /*!
     * @brief This function is used to stringtemplates to set the includes location.
     */
    public String getProduct();

    public String getNamespace();

    public boolean isCdr();

    public boolean isFastcdr();


    /*** End ***/

    /*** Java support block ***/

    public void setPackage(String pack);

    public boolean isIsPackageEmpty();

    public String getPackage();

    public String getPackageDir();

    public String getPackageUnder();

    /*** End ***/
}
