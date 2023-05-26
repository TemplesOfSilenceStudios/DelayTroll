/*
  ==============================================================================

    DTUnlockStatus.h
    Created: 6 Oct 2020 9:13:47am
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class DTUnlockStatus : public juce::OnlineUnlockStatus
{
public:
    
    DTUnlockStatus() = default;
    
    //==============================================================================
    /** This must return your product's ID, as allocated by the store. */
    juce::String getProductID() override;

    /** This must check whether a product ID string that the server returned is OK for
        unlocking the current app.
    */
    bool doesProductIDMatch (const juce::String& returnedIDFromServer) override;

    /** This must return the RSA public key for authenticating responses from
        the server for this app. You can get this key from your marketplace
        account page.
    */
    juce::RSAKey getPublicKey() override;

    /** This method must store the given string somewhere in your app's
        persistent properties, so it can be retrieved later by getState().
    */
    void saveState (const juce::String&) override;

    /** This method must retrieve the last state that was provided by the
        saveState method.

        On first-run, it should just return an empty string.
    */
    juce::String getState() override;

    /** Returns the name of the web-store website, not for communication, but for
        presenting to the user.
    */
    juce::String getWebsiteName()override;

    /** Returns the URL of the authentication API. */
    juce::URL getServerAuthenticationURL() override;

    /** Subclasses that talk to a particular web-store will implement this method
        to contact their webserver and attempt to unlock the current machine for
        the given username and password. The return value is the XML text from the
        server which contains error information and/or the encrypted keyfile.
    */
    juce::String readReplyFromWebserver (const juce::String& email, const juce::String& password) override;

    /**if the user cancels the attempt to unlock*/
    void userCancelled() override;
    
public:
    juce::CriticalSection mStreamCreationLock;
    std::unique_ptr<juce::WebInputStream> mStream;
};

class DTUnlockForm : public juce::OnlineUnlockForm
{
public:
    DTUnlockForm(DTUnlockStatus& status):
    OnlineUnlockForm(status, "Please provide your email and password.")
    {}
  
    void dismiss()override{
        setVisible(false);
    }
};
