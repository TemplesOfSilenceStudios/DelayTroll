/*
  ==============================================================================

    DTUnlockStatus.cpp
    Created: 6 Oct 2020 9:13:47am
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "DTUnlockStatus.h"

using namespace juce;

juce::String DTUnlockStatus::getProductID(){
    return "DelayTroll";
}

bool DTUnlockStatus::doesProductIDMatch (const juce::String& returnedIDFromServer){
    return getProductID() == returnedIDFromServer;
}

juce::RSAKey DTUnlockStatus::getPublicKey(){
    return juce::RSAKey("11,73a12f69fb0f6608169f2450b37d87542baaf6b2bc9e71cac64b062d749d4c8b");
}

void DTUnlockStatus::saveState (const juce::String&){}

juce::String DTUnlockStatus::getState(){return {};}

juce::String DTUnlockStatus::getWebsiteName(){
    return "juce.com";
}
juce::URL DTUnlockStatus::getServerAuthenticationURL(){
    return juce::URL("https://localhost:8443/auth.php");
}

juce::String DTUnlockStatus::readReplyFromWebserver (const juce::String& email, const juce::String& password){
    juce::URL url (getServerAuthenticationURL()
                .withParameter ("product", getProductID())
                .withParameter ("email", email)
                .withParameter ("pw", password)
                .withParameter ("os", juce::SystemStats::getOperatingSystemName())
                .withParameter ("mach", getLocalMachineIDs()[0]));

    DBG ("Trying to unlock via URL: " << url.toString (true));

    {
        juce::ScopedLock lock (mStreamCreationLock);
        mStream.reset (new juce::WebInputStream (url, true));
    }
    
    if (mStream->connect (nullptr))
    {
        auto* thread = juce::Thread::getCurrentThread();

        if (thread->threadShouldExit() || mStream->isError())
            return {};

        auto contentLength = mStream->getTotalLength();
        auto downloaded    = 0;

        const size_t bufferSize = 0x8000;
        juce::HeapBlock<char> buffer (bufferSize);

        while (! (mStream->isExhausted() || mStream->isError() || thread->threadShouldExit()))
        {
            auto max = juce::jmin ((int) bufferSize, contentLength < 0 ? std::numeric_limits<int>::max()
                                                                       : static_cast<int> (contentLength - downloaded));

            auto actualBytesRead = mStream->read (buffer.get() + downloaded, max - downloaded);

            if (actualBytesRead < 0 || thread->threadShouldExit() || mStream->isError())
                break;

            downloaded += actualBytesRead;

            if (downloaded == contentLength)
                break;
        }

        if (thread->threadShouldExit() || mStream->isError() || (contentLength > 0 && downloaded < contentLength))
            return {};

        return { juce::CharPointer_UTF8 (buffer.get()) };
    }

    return {};

}

void DTUnlockStatus::userCancelled()
{
    juce::ScopedLock lock (mStreamCreationLock);

    if (mStream != nullptr)
        mStream->cancel();
}
