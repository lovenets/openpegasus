//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
//%////////////////////////////////////////////////////////////////////////////

//
// This CIM client program is used to test the TestAggregationOutputProvider.
//

#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/CIMStatusCode.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/System.h>
#include <cstring>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

const CIMNamespaceName NAMESPACE   = CIMNamespaceName ("test/TestProvider");
const CIMName TEST_PERSON          = CIMName ("TEST_Person");
const CIMName TEST_TEACHES         = CIMName ("TEST_Teaches");
const CIMName TEST_WORKS           = CIMName ("TEST_Works");
const CIMName TEST_MARRIAGE        = CIMName ("TEST_Marriage");
const CIMName TEST_FAMILY          = CIMName ("TEST_Family");
const CIMName REPOSITORY_DEFAULT   =
    CIMName ("repositoryIsDefaultInstanceProvider");

// Variable used for collecting properties from CIMServer after the CIM
// Configuration
static const CIMName PROPERTY_NAME      = CIMName ("PropertyName");
static const CIMName DEFAULT_VALUE      = CIMName ("DefaultValue");
static const CIMName CURRENT_VALUE      = CIMName ("CurrentValue");
static const CIMName PLANNED_VALUE      = CIMName ("PlannedValue");
static const CIMName DYNAMIC_PROPERTY   = CIMName ("DynamicProperty");
Uint32 My_isDefaultInstanceProvider = 0;

static Boolean verbose = false;

////////////////////////////////////////////////////////////////////////////
//  _errorExit
////////////////////////////////////////////////////////////////////////////

void _errorExit(String message)
{
    cerr << "TestAggregationOutput error: " << message << endl;
    exit(1);
}

/////////////////////////////////////////////////////////////////////////////
//  Get Property from CIM Server
/////////////////////////////////////////////////////////////////////////////
// This would get the property values from CIMServer and return that for
// checking whether Repository Is Default InstanceProvider or not.

void getPropertiesFromCIMServer(
    CIMClient& client,
    const CIMName& propName,
    Array <String>& propValues)
{
    CIMProperty prop;

    Array<CIMKeyBinding> kbArray;
    CIMKeyBinding        kb;
    String               _hostName;

    kb.setName(PROPERTY_NAME);
    kb.setValue(propName.getString());
    kb.setType(CIMKeyBinding::STRING);

    _hostName.assign(System::getHostName());

    kbArray.append(kb);

    CIMObjectPath reference(_hostName, PEGASUS_NAMESPACENAME_CONFIG,
                            PEGASUS_CLASSNAME_CONFIGSETTING, kbArray);

    CIMInstance cimInstance = client.getInstance(PEGASUS_NAMESPACENAME_CONFIG,
                                                 reference);

    Uint32 pos = cimInstance.findProperty(PROPERTY_NAME);
    prop = (CIMProperty)cimInstance.getProperty(pos);
    propValues.append(prop.getValue().toString());

    pos = cimInstance.findProperty(DEFAULT_VALUE);
    prop = (CIMProperty)cimInstance.getProperty(pos);
    propValues.append(prop.getValue().toString());

    pos = cimInstance.findProperty(CURRENT_VALUE);
    prop = (CIMProperty)cimInstance.getProperty(pos);
    propValues.append(prop.getValue().toString());

    pos = cimInstance.findProperty(PLANNED_VALUE);
    prop = (CIMProperty)cimInstance.getProperty(pos);
    propValues.append(prop.getValue().toString());

    pos = cimInstance.findProperty(DYNAMIC_PROPERTY);
    prop = (CIMProperty)cimInstance.getProperty(pos);
    propValues.append(prop.getValue().toString());

}
////////////////////////////////////////////////////////////////////////////
//  _testAssociators
////////////////////////////////////////////////////////////////////////////
// This returns the total number of associators of the instances.

Uint32 _testAssociators(
    CIMClient& client,
    CIMName assocClass,
    CIMObjectPath instancePath)
{
    if (verbose)
    {
        cout << "\nAssociation Class: " << assocClass.getString() << endl;
        cout << "\nObject Name: " << instancePath.toString() << endl;
    }

    CIMName resultClass;
    String role;
    String resultRole;

    // Get the CIM instances that are associated with the specified source
    // instance via an instance of the AssocClass
    //
    Array<CIMObject> resultObjects =
        client.associators(NAMESPACE, instancePath, assocClass, resultClass,
                           role, resultRole);
    Uint32 size = resultObjects.size();
    if (verbose)
    {
        if (size > 0)
        {
            cout << " \n     " <<  assocClass.getString() << " :: ";
            for (Uint32 i = 0; i < size ; ++i)
            {
                cout << resultObjects[i].getPath().toString();
            }
            cout << endl;
        }
    }
    return size;
}

////////////////////////////////////////////////////////////////////////////
//  _testAssociatorNames
////////////////////////////////////////////////////////////////////////////
// This returns the total number of Associator Names of the instances.

Uint32 _testAssociatorNames(CIMClient& client,
                            CIMName assocClass,
                            CIMObjectPath instancePath)
{
    if (verbose)
    {
        cout << "\nAssociation Class: " << assocClass.getString() << endl;
        cout << "\nObject Name: " << instancePath.toString() << endl;
    }

    // Get the names of the CIM instances that are associated to the
    // specified source instance via an instance of the AssocClass.
    //
    CIMName resultClass;
    String role;
    String resultRole;
    Array<CIMObjectPath> resultObjectPaths = client.associatorNames(
        NAMESPACE, instancePath, assocClass, resultClass, role, resultRole);
    return resultObjectPaths.size();
}

////////////////////////////////////////////////////////////////////////////
//  _testReferences
////////////////////////////////////////////////////////////////////////////
// This returns the total number of References of the instances.

Uint32 _testReferences(
    CIMClient& client,
    CIMObjectPath instancePath,
    CIMName referenceClass)
{
    if (verbose)
    {
        cout << "\nObject Name: " << instancePath.toString() << endl;
    }

    // get the association reference instances
    //
    String role;
    Array<CIMObject> resultObjects =
        client.references(NAMESPACE, instancePath, referenceClass,role);
    Uint32 size = resultObjects.size();
    if (verbose)
    {
        if (size > 0)
        {
            cout << " \n     " <<  referenceClass.getString() << " :: ";
            for (Uint32 i = 0; i < size ; ++i)
            {
                cout << resultObjects[i].getPath().toString();
            }
            cout << endl;
        }
    }
    return size;
}

////////////////////////////////////////////////////////////////////////////
//  _testReferenceNames
////////////////////////////////////////////////////////////////////////////
// This returns the total number of Reference Names of the instances.

Uint32 _testReferenceNames(CIMClient& client,
                           CIMObjectPath instancePath,
                           CIMName referenceClass)
{
    if (verbose)
    {
        cout << "\nObject Name: " << instancePath.toString() << endl;
    }

    // get the reference instance names
    //
    String role;

    Array<CIMObjectPath> resultObjectPaths =
        client.referenceNames(NAMESPACE, instancePath, referenceClass, role);
    return resultObjectPaths.size();
}

////////////////////////////////////////////////////////////////////////////
//  _testaggregation
////////////////////////////////////////////////////////////////////////////
// This would test the aggregation of all the enumerateinstances,
// enumerateinstanceNames, associators, associatorNames, references,
// referenceNames of the TEST_Teaches class and its subclass
// TEST_TeachesDynamic. TEST_Teaches class has some static instances,
// associators.  TEST_TeachesDynamic has some Dynamic instances built by this
// TestAggregationOutputProvider.
// Output : As per the table in TestCaseReadMe.doc.

void _testaggregation(CIMClient& client)
{
    Uint32 a_count = 0;
    Uint32 an_count = 0;
    Uint32 r_count = 0;
    Uint32 rn_count = 0;

    cout << "\n+++++ Test aggregation of Static and Dynamic instances" << endl;
    try
    {
        Array<CIMObjectPath> teachesRefs;
        Array<CIMObjectPath> personRefs;
        Uint32 numteachesInstances ;
        Uint32 numpersonInstances ;

        teachesRefs = client.enumerateInstanceNames(NAMESPACE, TEST_TEACHES);
        numteachesInstances = teachesRefs.size();
        if (verbose)
        {
            cout << "     TEST_Teaches :: " ;
            cout << "     Number of instances " << numteachesInstances << endl;
        }

        // Checking the RepositoryIsDefaultInstanceProvider and the number
        // of instances returned.

        if (My_isDefaultInstanceProvider == 1 && 2 != numteachesInstances)
        {
            throw Exception(" Unexpected number of instances returned with "
                "Repository Enabled. ");
        }
        else if (My_isDefaultInstanceProvider == 0 && 1 != numteachesInstances)
        {
            throw Exception(" Unexpected number of instances returned with "
                "Repository Disabled. ");
        }

        personRefs = client.enumerateInstanceNames(NAMESPACE, TEST_PERSON);
        numpersonInstances = personRefs.size();

        for (Uint32 i = 0; i < numpersonInstances; ++i)
        {
            a_count  += _testAssociators(client, TEST_TEACHES, personRefs[i]);
            an_count +=
                _testAssociatorNames(client, TEST_TEACHES, personRefs[i]);
            r_count  += _testReferences(client, personRefs[i],TEST_TEACHES);
            rn_count +=
                _testReferenceNames(client, personRefs[i], TEST_TEACHES);
        }
        if (a_count != 2*numteachesInstances ||
            an_count != 2*numteachesInstances ||
            r_count != 2*numteachesInstances ||
            rn_count != 2*numteachesInstances)
        {
            throw Exception(" The number of instances returned is incorrect.");
        }
    }
    catch (Exception& e)
    {
        _errorExit(e.getMessage());
    }
    cout << "\n+++++ Test aggregation of Static and Dynamic instances passed"
         << endl;
}

////////////////////////////////////////////////////////////////////////////
//  _testFromRepository
////////////////////////////////////////////////////////////////////////////
// This would test the aggregation of all the enumerateinstances,
// enumerateinstanceNames, associators, associatorNames, references,
// referenceNames of the TEST_Works class and its subclass TEST_WorksDynamic.
// TEST_Works class has some static instances, associators and
// TEST_WorksDynamic does not have any dynamic isntances built and the
// TestAggregationOutputProvider returns the CIM_ERR_NOT_SUPPORTED for this
// class.
// Output : As per the table in TestCaseReadMe.doc.

void _testFromRepository(CIMClient& client)
{
    Uint32 a_count = 0;
    Uint32 an_count = 0;
    Uint32 r_count = 0;
    Uint32 rn_count = 0;

    cout << "\n+++++ Test for Static instance and Not supported Dynamic "
        "instance" << endl;
    try
    {
        Array<CIMObjectPath> worksRefs;
        Array<CIMObjectPath> personRefs;
        Uint32 numworksInstances;
        Uint32 numpersonInstances;

        worksRefs = client.enumerateInstanceNames(NAMESPACE, TEST_WORKS);
        numworksInstances = worksRefs.size();
        if (verbose)
        {
            cout << "     TEST_Works :: " ;
            cout << "     Number of instances " << numworksInstances << endl;
        }

        // Checking the RepositoryIsDefaultInstanceProvider and the number
        // of instances returned.

        if (My_isDefaultInstanceProvider == 1 && 1 != numworksInstances)
        {
            throw Exception(" Unexpected number of instances returned with "
                "Repository Enabled. ");
        }
        else if (My_isDefaultInstanceProvider == 0 && 0 != numworksInstances)
        {
            throw Exception(" Unexpected number of instances returned with "
                "Repository Disabled. ");
        }

        personRefs = client.enumerateInstanceNames(NAMESPACE, TEST_PERSON);
        numpersonInstances = personRefs.size();

        for (Uint32 i = 0; i < numpersonInstances; ++i)
        {
            a_count  += _testAssociators(client, TEST_WORKS, personRefs[i]);
            an_count += _testAssociatorNames(client, TEST_WORKS, personRefs[i]);
            r_count  += _testReferences(client, personRefs[i],TEST_WORKS);
            rn_count += _testReferenceNames(client, personRefs[i], TEST_WORKS);
        }
        if (a_count != 2*numworksInstances ||
            an_count != 2*numworksInstances ||
            r_count != 2*numworksInstances ||
            rn_count != 2*numworksInstances)
        {
            throw Exception(" The number of instances returned is incorrect.");
        }
    }
    catch (const CIMException& ex)
    {
        CIMStatusCode code = ex.getCode();
        if (CIM_ERR_NOT_SUPPORTED != code && CIM_ERR_NOT_FOUND != code)
        {
            throw Exception(" Test From Repository Failed. ");
        }
    }
    catch (Exception&)
    {
        throw Exception(" Test From Repository Failed. ");
    }
    cout << "\n+++++ Test for Static instance and Not supported Dynamic "
        "instance passed" << endl;
}

////////////////////////////////////////////////////////////////////////////
//  _testNotSupported
////////////////////////////////////////////////////////////////////////////
// This would test the aggregation of all the enumerateinstances,
// enumerateinstanceNames, associators, associatorNames, references,
// referenceNames of the TEST_Marriage class and its subclass
// TEST_MarriageDynamic. TEST_Marriage class does not have any static
// instances, associators. and also TEST_MarriageDynamic does not have any
// dynamic instances built. So the repository would return 0 instances and
// the TestAggregationOutputProvider would return the CIM_ERR_NOT_SUPPORTED
// exception for this class.
// Output : As per the table in TestCaseReadMe.doc.

void _testNotSupported(CIMClient& client)
{
    Uint32 a_count = 0;
    Uint32 an_count = 0;
    Uint32 r_count = 0;
    Uint32 rn_count = 0;

    cout << "\n+++++ Test for Not Supported Static and Dynamic instance"
         << endl;
    Array<CIMObjectPath> marriageRefs;
    Array<CIMObjectPath> personRefs;
    Uint32 nummarriageInstances = 0;
    Uint32 numpersonInstances ;

    try
    {
        marriageRefs = client.enumerateInstanceNames(NAMESPACE, TEST_MARRIAGE);
        nummarriageInstances = marriageRefs.size();
        if (verbose)
        {
            cout << "     TEST_Marriage :: " ;
            cout << "     Number of instances " << nummarriageInstances << endl;
        }

        // Checking the RepositoryIsDefaultInstanceProvider and the number
        // of instances returned.

        if (My_isDefaultInstanceProvider == 1 && 0 != nummarriageInstances)
        {
            throw Exception(" Unexpected number of instances returned with "
                "Repository Enabled. ");
        }
        else if (My_isDefaultInstanceProvider == 0 && 0 != nummarriageInstances)
        {
            throw Exception(" Unexpected number of instances returned with "
                "Repository Disabled. ");
        }
    }
    catch (const CIMException& ex)
    {
        CIMStatusCode code = ex.getCode();
        if (CIM_ERR_NOT_SUPPORTED != code)
        {
            throw Exception(" Test Not Supported Failed. ");
        }
    }
    catch (Exception &e)
    {
        cout << "ex message is " << e.getMessage() << endl;
        throw Exception(" Test Not Supported Failed. ");
    }
    try
    {
        personRefs = client.enumerateInstanceNames(NAMESPACE, TEST_PERSON);
        numpersonInstances = personRefs.size();

        for (Uint32 i = 0; i < numpersonInstances; ++i)
        {
            a_count  += _testAssociators(client, TEST_MARRIAGE, personRefs[i]);
            an_count +=
                _testAssociatorNames(client, TEST_MARRIAGE, personRefs[i]);
            r_count  += _testReferences(client, personRefs[i],TEST_MARRIAGE);
            rn_count +=
                _testReferenceNames(client, personRefs[i], TEST_MARRIAGE);
        }
        if (a_count != nummarriageInstances ||
            an_count != nummarriageInstances ||
            r_count != nummarriageInstances ||
            rn_count != nummarriageInstances)
        {
            throw Exception(" The number of instances returned is incorrect.");
        }
    }
    catch (const CIMException& ex)
    {
        CIMStatusCode code = ex.getCode();
        if (CIM_ERR_NOT_SUPPORTED != code)
        {
            throw Exception(" Test Not Supported Failed. ");
        }
    }
    catch (Exception& e)
    {
        cout << "ex message is "<<e.getMessage() << endl;
        throw Exception(" Test Not Supported Failed. ");
    }
    cout << "\n+++++ Test for Not Supported Static and Dynamic instance passed"
         << endl;
}

////////////////////////////////////////////////////////////////////////////
//  _testNotFound
////////////////////////////////////////////////////////////////////////////
// This would test the aggregation of all the enumerateinstances,
// enumerateinstanceNames, associators, associatorNames, references,
// referenceNames of the TEST_Family class and its subclass TEST_FamilyDynamic.
// TEST_Family class has some static instances, associators.  and
// TEST_FamilyDynamic does not have any dynamic instances built and the
// TestAggregationOutputProvider would return CIM_ERR_NOT_FOUND exception.
// So the output of methods on the class TEST_Family would return the static
// instances from the repository and the CIM_ERR_NOT_FOUND from the Provider.
// Output : As per the table in TestCaseReadMe.doc.

void _testNotFound(CIMClient& client)
{
    Uint32 a_count = 0;
    Uint32 an_count = 0;
    Uint32 r_count = 0;
    Uint32 rn_count = 0;

    cout << "\n+++++ Test for Supported Static and Not Found Dynamic instance"
         << endl;
    Array<CIMObjectPath> familyRefs;
    Array<CIMObjectPath> personRefs;
    Uint32 numfamilyInstances = 0;
    Uint32 numpersonInstances;

    try
    {
        familyRefs = client.enumerateInstanceNames(NAMESPACE, TEST_FAMILY);
        numfamilyInstances = familyRefs.size();
        if (verbose)
        {
            cout << "     TEST_Marriage :: " ;
            cout << "     Number of instances " << numfamilyInstances << endl;
        }

        // Checking the RepositoryIsDefaultInstanceProvider and the number
        // of instances returned.

        if (My_isDefaultInstanceProvider == 1 && 0 != numfamilyInstances)
        {
            throw Exception(" Unexpected number of instances returned with "
                "Repository Enabled. ");
        }
        else if (My_isDefaultInstanceProvider == 0 && 0 != numfamilyInstances)
        {
            throw Exception(" Unexpected number of instances returned with "
                "Repository Disabled. ");
        }
    }
    catch (const CIMException& ex)
    {
        CIMStatusCode code = ex.getCode();
        if (CIM_ERR_NOT_FOUND != code)
        {
            throw Exception(" Test Not Supported Failed. ");
        }
    }
    catch (Exception&)
    {
       throw Exception(" Test Not Supported Failed. ");
    }
    try
    {
        personRefs = client.enumerateInstanceNames(NAMESPACE, TEST_PERSON);
        numpersonInstances = personRefs.size();

        for (Uint32 i = 0; i < numpersonInstances; ++i)
        {
            a_count  += _testAssociators(client, TEST_FAMILY, personRefs[i]);
            an_count +=
                _testAssociatorNames(client, TEST_FAMILY, personRefs[i]);
            r_count  += _testReferences(client, personRefs[i],TEST_FAMILY);
            rn_count +=
                _testReferenceNames(client, personRefs[i], TEST_FAMILY);
        }
        if (a_count != numfamilyInstances ||
            an_count != numfamilyInstances ||
            r_count != numfamilyInstances ||
            rn_count != numfamilyInstances)
        {
            throw Exception(" The number of instances returned is incorrect.");
        }
    }
    catch (const CIMException& ex)
    {
        CIMStatusCode code = ex.getCode();
        if (CIM_ERR_NOT_FOUND != code)
        {
            throw Exception(" Test Not Supported Failed. ");
        }
    }
    catch (Exception&)
    {
        throw Exception(" Test Not Supported Failed. ");
    }
    cout << "\n+++++ Test for Supported Static and Not Found Dynamic instance "
        "Passed" << endl;
}


// =========================================================================
//    Main
// =========================================================================

int main(int argc, char** argv)
{
    // Variables to collect the CIMServer properties.
    Array<String> propertyValues;
    String currentValue;
    CIMClient client;

    // Check command line option

    if (argc > 2)
    {
        cerr << "Usage: TestAggregationOutputClient [-v]" << endl;
        return 1;
    }

    if (2 == argc)
    {
        const char *opt = argv[1];
        if (0 == strcmp(opt, "-v"))
        {
            verbose = true;
        }
        else
        {
            cerr << "Usage: TestAggregationOutputClient [-v]" << endl;
            return 1;
        }
    }

    // Connect to server
    try
    {
        client.connectLocal();
    }
    catch (Exception& e)
    {
        _errorExit(e.getMessage());
    }

    // Get the Property value from CIMServer with REPOSITORY_DEFAULT

    getPropertiesFromCIMServer(client, REPOSITORY_DEFAULT, propertyValues);
    currentValue = propertyValues[2];

    if (currentValue == "true")
       My_isDefaultInstanceProvider = 1;

    cout << "+++++ Test AggregationOutput Provider" << endl;
    cout << endl;

    if (verbose)
    {
        cout << "+++++ Testing with repositoryIsDefaultInstanceProvider =  "
             << My_isDefaultInstanceProvider << endl;
    }
    try
    {
        _testaggregation(client);
        _testFromRepository(client);
        _testNotSupported(client);
        _testNotFound(client);
    }
    catch (Exception&)
    {
        cout << "\n----- Test Aggregation Failed" << endl;
    }
    // Disconnect from server
    try
    {
        client.disconnect();
    }
    catch (Exception& e)
    {
        _errorExit(e.getMessage());
    }

    return 0;
}
