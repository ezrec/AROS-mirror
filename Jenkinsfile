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


def buildStep(ext, iconset = 'default', binutilsver = '2.30', gccver = '6.3.0') {
	stage("Building ${ext} with gcc ${gccver} and binutils ${binutilsver}...") {
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

	    stage("Starting $ext build target...") {
	      slackSend color: "good", channel: "#jenkins", message: "Starting ${ext} build target..."
	    }

	    stage('Freshing up the root') {
	      freshUpRoot(ext, binutilsver, gccver)
	    }

	    stage('Configuring...') {
	      sh "cd build-${ext}-${gccver}-${binutilsver} && ../AROS/configure --target=${ext} --enable-ccache --with-iconset=${iconset} --enable-build-type=nightly --with-serial-debug --with-binutils-version=${binutilsver} --with-gcc-version=${gccver} --with-aros-toolchain-install=${env.WORKSPACE}/tools-${gccver}-${binutilsver} --with-portssources=${env.WORKSPACE}/externalsources"
	    }

	    stage('Building AROS main source...') {
	      sh "cd build-${ext}-${gccver}-${binutilsver} && make"

	    }

	    stage('Copy over contrib...') {
	      postCoreBuild(ext)
	    }

	    stage('Building selected AROS contributions...') {
	      sh "cd build-${ext}-${gccver}-${binutilsver} && make contrib-installerlg"
	    }

	    stage('Making distfiles...') {
	      sh "cd build-${ext}-${gccver}-${binutilsver} && make distfiles"
	    }

	    if (!env.CHANGE_ID) {
	      stage('Moving dist files for publishing') {
		sh "mkdir -p publishing/deploy/aros/${ext}-${gccver}-${binutilsver}/"
		sh "echo '${ext}-${gccver}-${binutilsver},' > publishing/deploy/aros/TARGETS"
		sh "cp -fvr build-${ext}-${gccver}-${binutilsver}/distfiles/* publishing/deploy/aros/${ext}-${gccver}-${binutilsver}/"
		sh "cp -pRL AROS/LICENSE publishing/deploy/aros/${ext}-${gccver}-${binutilsver}/"
		sh "cp -pRL AROS/ACKNOWLEDGEMENTS publishing/deploy/aros/${ext}-${gccver}-${binutilsver}/"
		sh "rm -rfv publishing/deploy/aros/${ext}-${gccver}-${binutilsver}/*.elf"
	      }
	    }

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

				slackSend color: "good", channel: "#aros", message: "Deploying ${env.JOB_NAME} #${env.BUILD_NUMBER} Target: ${ext} GCC: ${gccver} Binutils: ${binutilsver} to web (<https://www.eevul.net/${deploy_url}|https://www.eevul.net/${deploy_url}>)"
			} else if (env.BRANCH_NAME.equals('ABI_V1_experimental')) {
				def deploy_url = sh (
				    script: 'echo "/downloads/nightly/aros-experimental/`date +\'%Y\'`/`date +\'%m\'`/`date +\'%d\'`/"',
				    returnStdout: true
				).trim()
				sh "date +'%Y-%m-%d %H:%M:%S' > publishing/deploy/BUILDTIME"
				sh "ssh $DEPLOYHOST mkdir -p public_html/downloads/nightly/aros-experimental/`date +'%Y'`/`date +'%m'`/`date +'%d'`/"
				sh "scp -r publishing/deploy/aros/* $DEPLOYHOST:~/public_html/downloads/nightly/aros-experimental/`date +'%Y'`/`date +'%m'`/`date +'%d'`/"
				sh "scp publishing/deploy/BUILDTIME $DEPLOYHOST:~/public_html/downloads/nightly/aros-experimental/"

				slackSend color: "good", channel: "#aros", message: "Deploying ${env.JOB_NAME} #${env.BUILD_NUMBER} Target: ${ext} GCC: ${gccver} Binutils: ${binutilsver} to web (<https://www.eevul.net/${deploy_url}|https://www.eevul.net/${deploy_url}>)"
			} else {
				slackSend color: "good", channel: "#aros", message: "Build ${env.JOB_NAME} #${env.BUILD_NUMBER} Target: ${ext} GCC: ${gccver} Binutils: ${binutilsver} successful!"
			}
		}
	}
}

def freshUpRoot(ext, binutilsver, gccver) {
	sh "rm -rfv build-${ext}-${gccver}-${binutilsver}/distfiles/*"
	// uncomment the following section to remove the whole toolchain and build: 
	// sh "rm -rfv ${env.WORKSPACE}/tools"
	// sh "rm -rfv ${env.WORKSPACE}/build-$ext/*"
	// end of section
	sh "rm -rfv AROS/contrib"
	sh "rm -rfv AROS/ports"

	sh "mkdir -p build-${ext}-${gccver}-${binutilsver}"
  	sh "mkdir -p externalsources"
	sh "mkdir -p tools-${gccver}-${binutilsver}"
}

def postCoreBuild(ext) {
	sh "cp -fvr contrib AROS/"
	sh "cp -fvr ports AROS/"
}

node {
	try{
		slackSend color: "good", channel: "#jenkins", message: "Build Started: ${env.JOB_NAME} #${env.BUILD_NUMBER} (<${env.BUILD_URL}|Open>)"
		parallel (
			'Build Amiga 68k version - GCC 6.3.0 - Binutils 2.30': {
				node {			
					buildStep('amiga-m68k', 'default', '2.30', '6.3.0')
				}
			},
			'Build Amiga 68k version - GCC 6.5.0 - Binutils 2.32': {
				node {			
					buildStep('amiga-m68k', 'default', '2.32', '6.5.0')
				}
			}
		)
			
	} catch(err) {
		slackSend color: "danger", channel: "#aros", message: "Build Failed: ${env.JOB_NAME} #${env.BUILD_NUMBER} (<${env.BUILD_URL}|Open>)"
		currentBuild.result = 'FAILURE'
		notify('Build failed')
		throw err
	}
}
