# Amazon Polly TTS Engine for Windows
> Use Amazon Polly voices in native Windows TTS applications

A brief description of your project, what it is used for and how does life get
awesome when someone starts to use it.

## QuickStart

## Step 1: Install the AWS Client (*if it's not already installed on your machine*) ##

Follow the instructions on [this page](http://docs.aws.amazon.com/cli/latest/userguide/awscli-install-windows.html) to install the AWS Command Line Interface on Microsoft Windows.

> **IMPORTANT:** If you're not sure what to do here, follow the instructions in the `MSI Installer` section.

## Step 2: Setup your AWS user account ##

1. Login to [your AWS account](https://console.aws.amazon.com/console/home)
2. After you've logged in, click `Services` from the top menu bar, then type `iam` in the search box. Click `IAM` when it pops up.
4. On the left, click `Users`
5. Click `Add User`
6. Type in `polly-windows-user` (you can use any name)
7. Click the `Programmatic access` checkbox and leave `AWS Management Console access` unchecked
8. Click `Next: Permissions`
9. Click `Attach existing policies directly`
10. At the bottom of the page, in the search box next to `Filter: Policy type`, type `polly`
11. Click the checkbox next to `AmazonPollyFullAccess`
12. Click `Next: Review`
13. Click `Create user`
> **IMPORTANT:** *Don't close the web page. You'll need both the `Access key ID` and the `Secret access key` in the next step.*


## Step 3: Configure the AWS Client
> Amazon Polly TTS Engine for Windows requires an AWS profile called `polly-windows`. It's in the roadmap to allow custom profiles. 

1. Open a Windows command prompt.
2. Type this command:

         aws configure --profile polly-windows

3. For `AWS Access Key ID` and `AWS Secret Access Key`, use the values from the previous step.
4. For `Default region name`, you can hit `Enter` for the default (`us-east-1`) or enter a different region. **Make sure to use all lower-case. This is case-sensitive.**
5. For `Default output format`, just hit `Enter`
6. Verify this worked by running this command. You should see a list of voices:

         aws --profile polly-windows polly describe-voices

## Step 4: Install Amazon Polly TTS Engine for Windows
Download and run the [installer](https://s3.amazonaws.com/polly-tts-windows/setup.exe)

Verify that the installer worked by opening `Control Panel` and go to `Change text to speech settings`. In the `Voice selection` drop-down, you should see all of the Amazon Polly voices. Picking a voice will automatically play a sample.

## Adobe Captivate Support
Even though there is a drop-down voice selection in the `Audio / Speech Management` window, apparently Adobe Captivate only uses the default voice you choose in Windows Control Panel. The voice selection in Captivate is completely ignored.

To get around this, you can manually specify the voice name that you want to use by using the `voice` tag. 
 
### Instructions for Manually Specifying a Voice in Adobe Captivate ###

1.	Set the default Windows voice in Control Panel to **any** of the Amazon Polly voices.
2.	Start Adobe Captivate and open any of your previous documents.
3.	Choose `Speech Management` from the `Audio` menu
4.	(See the screenshot below): Where you enter the text to speak, put the highlighted yellow tags around the text, replacing the name with the voice name you want to use. For example, `<speak><voice name="Matthew">Hi, this is Matthew</voice></speak>` will automatically speak the phrase *Hi, this is Matthew* in the **Matthew** voice and  `<speak><voice name="Ivy">Hi, this is Ivy</voice></speak>` will say *Hi, this is Ivy* in the **Ivy** voice.
 
Click [here](http://docs.aws.amazon.com/polly/latest/dg/voicelist.html) for the complete list of Polly voice names.
 
> **IMPORTANT:** It doesn’t matter which voice you select at the top or the left side menu, as long as it’s a Polly voice. The <voice> tag will override those selections. However, if you choose a non-Polly voice as the Windows default voice, then the <voice> tag will NOT work with the Polly voices.
 
Also, you can only use one `<speak>` tag per block of text, and you can only use one `<voice>` tag per `<speak>` tag (e.g., you can’t do `<speak><voice name="ivy">I’m Ivy.</voice><voice name="Matthew">I’m Matthew</voice></speak>`).

![](https://i.imgur.com/LMlNszU.png)