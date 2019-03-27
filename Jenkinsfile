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

@NonCPS
def killall_jobs() {
	def jobname = env.JOB_NAME
	def buildnum = env.BUILD_NUMBER.toInteger()
	def killnums = ""
	def job = Jenkins.instance.getItemByFullName(jobname)
	for (build in job.builds) {
		if (!build.isBuilding()) { continue; }
		if (buildnum == build.getNumber().toInteger()) { continue; println "equals" }
		if (buildnum < build.getNumber().toInteger()) { continue; println "newer" }
		
		echo "Kill task = ${build}"
		
		killnums += "#" + build.getNumber().toInteger() + ", "
		
		build.doStop();
	}
	
	if (killnums != "") {
		slackSend color: "danger", channel: "#aros", message: "Killing task(s) ${env.JOB_NAME} ${killnums} in favor of #${buildnum}, ignore following failed builds for ${killnums}"
	}
	echo "Done killing"
}

def buildStep(ext, iconset = 'default', binutilsver = '2.30', gccver = '6.3.0', nativetarget = true) {
	try{
		stage("Building ${ext} with gcc ${gccver} and binutils ${binutilsver}...") {
			properties([pipelineTriggers([githubPush()])])
			if (env.CHANGE_ID) {
				echo 'Trying to build pull request'
			}

			def commondir = ${env.JENKINS_HOME} + '/' + ${env.JOB_NAME} + '/'

			checkout scm

			if (!env.CHANGE_ID) {
				sh "rm -rfv ${env.WORKSPACE}/publishing/deploy/*"
				sh "mkdir -p ${env.WORKSPACE}/publishing/deploy/aros"
			}

			slackSend color: "good", channel: "#jenkins", message: "Starting ${ext} build target..."

			freshUpRoot(ext, binutilsver, gccver)

			sh "cd ${env.WORKSPACE}/build-${ext}-${gccver}-${binutilsver} && ${env.WORKSPACE}/AROS/configure --target=${ext} --enable-ccache --with-iconset=${iconset} --enable-build-type=nightly --with-serial-debug --with-binutils-version=${binutilsver} --with-gcc-version=${gccver} --with-aros-toolchain-install=${commondir}/tools/tools-${ext}-${gccver}-${binutilsver} --with-portssources=${commondir}/externalsources"

			sh "cd ${env.WORKSPACE}/build-${ext}-${gccver}-${binutilsver} && make"

			if (!nativetarget) {
				sh "cd ${env.WORKSPACE}/build-${ext}-${gccver}-${binutilsver} && make default-x11keymaptable"
			}

			postCoreBuild(ext)

			sh "cd ${env.WORKSPACE}/build-${ext}-${gccver}-${binutilsver} && make contrib-installerlg"

			sh "cd ${env.WORKSPACE}/build-${ext}-${gccver}-${binutilsver} && make distfiles"

			if (!env.CHANGE_ID) {
				sh "mkdir -p ${env.WORKSPACE}/publishing/deploy/aros/${ext}-${gccver}-${binutilsver}/"
				sh "echo '${env.BUILD_NUMBER}' > ${env.WORKSPACE}/publishing/deploy/aros/${ext}-${gccver}-${binutilsver}/BUILD"
				sh "cp -pRL ${env.WORKSPACE}/AROS/LICENSE ${env.WORKSPACE}/publishing/deploy/aros/${ext}-${gccver}-${binutilsver}/"
				sh "cp -pRL ${env.WORKSPACE}/AROS/ACKNOWLEDGEMENTS ${env.WORKSPACE}/publishing/deploy/aros/${ext}-${gccver}-${binutilsver}/"

				if (nativetarget) {					
					sh "cp -fvr ${env.WORKSPACE}/build-${ext}-${gccver}-${binutilsver}/distfiles/* ${env.WORKSPACE}/publishing/deploy/aros/${ext}-${gccver}-${binutilsver}/"

					//sh "rm -rfv ${env.WORKSPACE}/publishing/deploy/aros/${ext}-${gccver}-${binutilsver}/*.elf" // Can't remember what this is good for...
				} else {
					sh "cp -fvr ${env.WORKSPACE}/build-${ext}-${gccver}-${binutilsver}/bin/${ext}/AROS ${env.WORKSPACE}/publishing/deploy/aros/${ext}-${gccver}-${binutilsver}/"
					sh "cd ${env.WORKSPACE}/publishing/deploy/aros/${ext}-${gccver}-${binutilsver}/ && tar -Jcvvf ${ext}-hosted.tar.xz *"
					sh "rm -fvr ${env.WORKSPACE}/publishing/deploy/aros/${ext}-${gccver}-${binutilsver}/AROS"
				}
			}

			if (env.TAG_NAME) {
				sh "echo $TAG_NAME > ${env.WORKSPACE}/publishing/deploy/STABLE"
				sh "ssh $DEPLOYHOST mkdir -p public_html/downloads/releases/aros/$TAG_NAME"
				sh "scp -r ${env.WORKSPACE}/publishing/deploy/aros/* $DEPLOYHOST:~/public_html/downloads/releases/aros/$TAG_NAME/"
				sh "scp ${env.WORKSPACE}/publishing/deploy/STABLE $DEPLOYHOST:~/public_html/downloads/releases/aros/"
			} else if (env.BRANCH_NAME.equals('ABI_V1')) {
				def deploy_url = sh (
				    script: 'echo "nightly/aros/`date +\'%Y\'`/`date +\'%m\'`/`date +\'%d\'`/"',
				    returnStdout: true
				).trim()
				sh "date +'%Y-%m-%d %H:%M:%S' > ${env.WORKSPACE}/publishing/deploy/BUILDTIME"
				sh "ssh $DEPLOYHOST mkdir -p public_html/downloads/nightly/aros/`date +'%Y'`/`date +'%m'`/`date +'%d'`/"
				sh "scp -r ${env.WORKSPACE}/publishing/deploy/aros/* $DEPLOYHOST:~/public_html/downloads/nightly/aros/`date +'%Y'`/`date +'%m'`/`date +'%d'`/"
				sh "scp ${env.WORKSPACE}/publishing/deploy/BUILDTIME $DEPLOYHOST:~/public_html/downloads/nightly/aros/"

				slackSend color: "good", channel: "#aros", message: "Deploying ${env.JOB_NAME} #${env.BUILD_NUMBER} Target: ${ext} GCC: ${gccver} Binutils: ${binutilsver} to web (<https://dl.amigadev.com/${deploy_url}|https://dl.amigadev.com/${deploy_url}>)"
			} else if (env.BRANCH_NAME.equals('ABI_V1_experimental')) {
				def deploy_url = sh (
				    script: 'echo "nightly/aros-experimental/`date +\'%Y\'`/`date +\'%m\'`/`date +\'%d\'`/"',
				    returnStdout: true
				).trim()
				sh "date +'%Y-%m-%d %H:%M:%S' > ${env.WORKSPACE}/publishing/deploy/BUILDTIME"
				sh "ssh $DEPLOYHOST mkdir -p public_html/downloads/nightly/aros-experimental/`date +'%Y'`/`date +'%m'`/`date +'%d'`/"
				sh "scp -r ${env.WORKSPACE}/publishing/deploy/aros/* $DEPLOYHOST:~/public_html/downloads/nightly/aros-experimental/`date +'%Y'`/`date +'%m'`/`date +'%d'`/"
				sh "scp ${env.WORKSPACE}/publishing/deploy/BUILDTIME $DEPLOYHOST:~/public_html/downloads/nightly/aros-experimental/"

				slackSend color: "good", channel: "#aros", message: "Deploying ${env.JOB_NAME} #${env.BUILD_NUMBER} Target: ${ext} GCC: ${gccver} Binutils: ${binutilsver} to web (<https://dl.amigadev.com/${deploy_url}|https://dl.amigadev.com/${deploy_url}>)"
			} else {
				slackSend color: "good", channel: "#aros", message: "Build ${env.JOB_NAME} #${env.BUILD_NUMBER} Target: ${ext} GCC: ${gccver} Binutils: ${binutilsver} successful!"
			}
		}
	} catch(err) {
		slackSend color: "danger", channel: "#aros", message: "Build Failed: ${env.JOB_NAME} #${env.BUILD_NUMBER} Target: ${ext} GCC: ${gccver} (<${env.BUILD_URL}|Open>)"
		currentBuild.result = 'FAILURE'
		notify('Build failed')
		throw err
	}
}

def freshUpRoot(ext, binutilsver, gccver) {
	def commondir = ${env.JENKINS_HOME} + '/' + ${env.JOB_NAME} + '/'
	sh "rm -rfv ${env.WORKSPACE}/build-${ext}-${gccver}-${binutilsver}/distfiles/*"
	// uncomment the following section to remove the whole toolchain and build: 
	// sh "rm -rfv ${commondir}/tools/tools-${ext}-${gccver}-${binutilsver}"
	// sh "rm -rfv ${env.WORKSPACE}/build-${ext}-${gccver}-${binutilsver}/*"
	// end of section
	sh "rm -rfv ${env.WORKSPACE}/AROS/contrib"
	sh "rm -rfv ${env.WORKSPACE}/AROS/ports"
	
	sh "mkdir -p ${env.WORKSPACE}/build-${ext}-${gccver}-${binutilsver}"
	sh "mkdir -p ${commondir}/externalsources"
	sh "mkdir -p ${commondir}/tools/tools-${ext}-${gccver}-${binutilsver}"
}

def postCoreBuild(ext) {
	sh "cp -fvr ${env.WORKSPACE}/contrib ${env.WORKSPACE}/AROS/"
	sh "cp -fvr ${env.WORKSPACE}/ports ${env.WORKSPACE}/AROS/"
}

node('master') {
	killall_jobs()
	slackSend color: "good", channel: "#jenkins", message: "Build Started: ${env.JOB_NAME} #${env.BUILD_NUMBER} (<${env.BUILD_URL}|Open>)"
	parallel (
		'Build Amiga 68k version - GCC 8.3.0 - Binutils 2.32': {
			node {			
				buildStep('amiga-m68k', 'default', '2.32', '8.3.0')
			}
		},
		'Build Amiga 68k version - GCC 6.5.0 - Binutils 2.32': {
			node {			
				buildStep('amiga-m68k', 'default', '2.32', '6.5.0')
			}
		},
		'Build Linux Hosted x86_64 version - GCC 8.3.0 - Binutils 2.32': {
			node {			
				buildStep('linux-x86_64', 'default', '2.32', '8.3.0', false)
			}
		}
	)
}
