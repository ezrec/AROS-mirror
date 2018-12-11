def notify(status){
	emailext (
		body: '$DEFAULT_CONTENT', 
		recipientProviders: [
			[$class: 'CulpritsRecipientProvider'],
			[$class: 'DevelopersRecipientProvider'],
			[$class: 'RequesterRecipientProvider']
		], 
		replyTo: '$DEFAULT_REPLYTO', 
		subject: '$DEFAULT_SUBJECT',
		to: '$DEFAULT_RECIPIENTS'
	)
}

def buildStep(ext) {
	stage("Starting $ext build target...") {
		slackSend color: "good", channel: "#aros", message: "Starting ${ext} build target..."
	}

	stage('Freshing up the root') {
		freshUpRoot(ext)
	}
	
	stage('Configuring...') {
		sh "cd build-$ext && ../AROS/configure --target=$ext --enable-build-type=nightly --with-serial-debug --enable-ccache --with-aros-toolchain-install=${env.WORKSPACE}/tools --with-portssources=${env.WORKSPACE}/externalsources"
	}
	
	stage('Building AROS main source...') {
		sh "cd build-$ext && make"
	}
	
	stage('Building selected AROS contributions...') {
		sh "cd build-$ext && make contrib-installerlg"
	}
	
	stage('Making distfiles...') {
		sh "cd build-$ext && make distfiles"
	}
	
	if (!env.CHANGE_ID) {
		stage('Moving dist files for publishing') {
			sh "mkdir -p publishing/deploy/aros/$ext/"
			sh "echo '$ext,' > publishing/deploy/aros/TARGETS"
			sh "cp -fvr build-$ext/distfiles/* publishing/deploy/aros/$ext/"
			sh "cp -pRL AROS/LICENSE publishing/deploy/aros/$ext/"
			sh "cp -pRL AROS/ACKNOWLEDGEMENTS publishing/deploy/aros/$ext/"
			sh "rm -rfv publishing/deploy/aros/$ext/*.elf"
		}
	}
}

def freshUpRoot(ext) {
	sh "rm -rfv build-$ext/distfiles/*"
	sh "rm -rfv ${env.WORKSPACE}/tools"
	sh "rm -rfv ${env.WORKSPACE}/build-$ext/bin/*/Ports"
	sh "rm -rfv ${env.WORKSPACE}/build-$ext/bin/*/gen"
	sh "rm -rfv AROS/contrib"
	sh "rm -rfv AROS/ports"
	sh "mv -fv contrib AROS/"
	sh "mv -fv ports AROS/"
	sh "mkdir -p build-$ext"
  	sh "mkdir -p externalsources"
	sh "mkdir -p tools"
}

node {
	try{
		slackSend color: "good", channel: "#aros", message: "Build Started: ${env.JOB_NAME} #${env.BUILD_NUMBER} (<${env.BUILD_URL}|Open>)"
		
		stage('Checkout and pull') {
			properties([pipelineTriggers([githubPush()])])
			if (env.CHANGE_ID) {
				echo 'Trying to build pull request'
			}

			checkout scm
		}
	
		if (!env.CHANGE_ID) {
			stage('Generate publishing directories') {
				sh "rm -rfv publishing/deploy/*"
				sh "mkdir -p publishing/deploy/aros"
			}
		}
		
		buildStep('amiga-m68k')
		
		stage('Deploying to stage') {
			if (env.TAG_NAME) {
				sh "echo $TAG_NAME > publishing/deploy/STABLE"
				sh "ssh $DEPLOYHOST mkdir -p public_html/downloads/releases/aros/$TAG_NAME"
				sh "scp -r publishing/deploy/aros/* $DEPLOYHOST:~/public_html/downloads/releases/aros/$TAG_NAME/"
				sh "scp publishing/deploy/STABLE $DEPLOYHOST:~/public_html/downloads/releases/aros/"
			} else if (env.BRANCH_NAME.equals('ABI_V1')) {
				def deploy_url = sh (
				    script: 'echo "/downloads/nightly/aros/`date +\'%Y\'`/`date +\'%m\'`/`date +\'%d\'`/"',
				    returnStdout: true
				).trim()
				sh "date +'%Y-%m-%d %H:%M:%S' > publishing/deploy/BUILDTIME"
				sh "ssh $DEPLOYHOST mkdir -p public_html/downloads/nightly/aros/`date +'%Y'`/`date +'%m'`/`date +'%d'`/"
				sh "scp -r publishing/deploy/aros/* $DEPLOYHOST:~/public_html/downloads/nightly/aros/`date +'%Y'`/`date +'%m'`/`date +'%d'`/"
				sh "scp publishing/deploy/BUILDTIME $DEPLOYHOST:~/public_html/downloads/nightly/aros/"
				
				slackSend color: "good", channel: "#aros", message: "Deploying to web (<https://www.eevul.net/${deploy_url}|Open>)"
			}
		}
	
	} catch(err) {
		slackSend color: "danger", channel: "#aros", message: "Build Failed: ${env.JOB_NAME} #${env.BUILD_NUMBER} (<${env.BUILD_URL}|Open>)"
		currentBuild.result = 'FAILURE'
		notify('Build failed')
		throw err
	}
}
