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

def workspace = pwd() 

def buildStep(ext) {
	stage("Starting $ext build target...") {
			
	}

	stage('Freshing up the root') {
		freshUpRoot(ext)
	}
	
	stage('Configuring...') {
		sh "cd build-$ext && ../AROS/configure --target=$ext --enable-build-type=nightly --with-serial-debug --with-binutils-version=2.30 --with-gcc-version=6.3.0 --with-portssources=${env.WORKSPACE}/externalsources"
	}
	
	stage('Building...') {
		sh "cd build-$ext && make"
	}
	
	stage('Making distfiles...') {
		sh "cd build-$ext && make distfiles"
	}
	
	if (!env.CHANGE_ID) {
		stage('Moving dist files for publishing') {
			sh "mkdir -p publishing/deploy/aros/$ext/"
			sh "echo '$ext,' > publishing/deploy/TARGETS"
			sh "cp -fvr build-$ext/distfiles publishing/deploy/aros/$ext/"
		}
	}
}

def freshUpRoot(ext) {
	sh "rm -rfv build-$ext"
	sh "mkdir -p build-$ext"
  	sh "mkdir -p externalsources"
}

node {
	try{
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
				sh "date +'%Y-%m-%d %H:%M:%S' > publishing/deploy/BUILDTIME"
				sh "ssh $DEPLOYHOST mkdir -p public_html/downloads/nightly/aros/`date +'%Y'`/`date +'%m'`/`date +'%d'`/"
				sh "scp -r publishing/deploy/aros/* $DEPLOYHOST:~/public_html/downloads/nightly/aros/`date +'%Y'`/`date +'%m'`/`date +'%d'`/"
				sh "scp publishing/deploy/BUILDTIME $DEPLOYHOST:~/public_html/downloads/nightly/aros/"
			}
		}
	
	} catch(err) {
		currentBuild.result = 'FAILURE'
		notify('Build failed')
		throw err
	}
}
