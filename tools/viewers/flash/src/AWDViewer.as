package
{
	import away3d.animators.SkeletonAnimator;
	import away3d.arcane;
	import away3d.containers.View3D;
	import away3d.controllers.HoverController;
	import away3d.entities.Mesh;
	import away3d.events.AssetEvent;
	import away3d.events.LoaderEvent;
	import away3d.library.assets.AssetType;
	import away3d.lights.DirectionalLight;
	import away3d.lights.PointLight;
	import away3d.loaders.Loader3D;
	import away3d.loaders.parsers.AWD2Parser;
	import away3d.materials.ColorMaterial;
	import away3d.materials.TextureMaterial;
	import away3d.materials.lightpickers.StaticLightPicker;
	import away3d.primitives.SkyBox;
	
	import flash.display.Sprite;
	import flash.display.StageAlign;
	import flash.display.StageScaleMode;
	import flash.events.Event;
	import flash.events.MouseEvent;
	import flash.geom.Vector3D;
	import flash.net.URLRequest;
	
	use namespace arcane;
	
	[SWF(width="800", height="600")]
	public class AWDViewer extends Sprite
	{
		private var _view : View3D;
		private var _loader : Loader3D;
		
		private var _hoverCtrl : HoverController;
		private var _prevMouseX : Number;
		private var _prevMouseY : Number;
						
		public function AWDViewer()
		{
			init();
			load(loaderInfo.parameters['awd'] || 'default.awd');
			
			// Force resize
			stage.dispatchEvent(new Event(Event.RESIZE));
		}
		
		
		private function init() : void
		{
			_view = new View3D();
			_view.antiAlias = 4;
			_view.camera.lens.far = 5000;
			_view.backgroundColor = parseInt(loaderInfo.parameters['bgcolor'],16) || 0;
			addChild(_view);
			addEventListener(Event.ENTER_FRAME, onEnterFrame);
			
			_hoverCtrl = new HoverController(_view.camera);
			_hoverCtrl.tiltAngle = 0;
			_hoverCtrl.panAngle = 180;
			_hoverCtrl.minTiltAngle = -60;
			_hoverCtrl.maxTiltAngle = 60;
						
			stage.addEventListener(MouseEvent.MOUSE_DOWN, onStageMouseDown);
			stage.addEventListener(MouseEvent.MOUSE_MOVE, onStageMouseMove);
			stage.addEventListener(MouseEvent.MOUSE_WHEEL, onStageMouseWheel);
			
			stage.align = StageAlign.TOP_LEFT;
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.addEventListener(Event.RESIZE, onStageResize);
		}
		
		
		private function load(url : String) : void
		{
			Loader3D.enableParser(AWD2Parser);
			_loader = new Loader3D(true);
			_loader.addEventListener(AssetEvent.ASSET_COMPLETE, onAssetComplete);
			_loader.addEventListener(LoaderEvent.RESOURCE_COMPLETE, onResourceComplete);
			_loader.load(new URLRequest(url));
		}
		
		
		private var _i : uint = 0;
		
		private function onAssetComplete(ev : AssetEvent) : void
		{
			if (ev.asset.assetType == AssetType.MESH) {
				trace("Asset: Mesh = " + ev.asset.name);
			}
			else if (ev.asset.assetType == AssetType.SKELETON) {		
				trace("Asset: Skeleton = "+ev.asset.name);
				//_skeleton = Skeleton(ev.asset);
			}
			else if (ev.asset.assetType == AssetType.SKYBOX) {	
				var thisSkyBox:SkyBox = SkyBox(ev.asset);
				trace("Asset: SKYBOX = " + ev.asset.name);
				_view.scene.addChild(thisSkyBox);
			}
			else if (ev.asset.assetType == AssetType.MATERIAL) {
				trace("Asset: MATERIAL = "+ev.asset.name);
				//trace(_i++, ev.asset.name, ev.asset.assetType, ev.asset);
			}
			else if (ev.asset.assetType == AssetType.LIGHT) {
				trace("Asset: LIGHT = "+ev.asset.name);
				//trace(_i++, ev.asset.name, ev.asset.assetType, ev.asset);
			}
			else if (ev.asset.assetType == AssetType.TEXTURE) {		
				trace("Asset: TEXTURE = "+ev.asset.name);
				//_skeleton = Skeleton(ev.asset);
			}
			else if (ev.asset.assetType == AssetType.ANIMATOR) {		
				trace("Asset: ANIMATOR = " + ev.asset.name);
			}
		}
		
		
		private function onResourceComplete(ev : LoaderEvent) : void
		{
			var scale : Number;
			var max : Vector3D;
			var min : Vector3D;
			var d : Vector3D;
				
			max = new Vector3D(_loader.maxX, _loader.maxY, _loader.maxZ);
			min = new Vector3D(_loader.minX, _loader.minY, _loader.minZ);
			d = max.subtract(min);
			
			scale = 300 / d.length;
			_hoverCtrl.distance *=  scale;
			_view.scene.addChild(_loader);			
			
			// Force resize
			stage.dispatchEvent(new Event(Event.RESIZE));
		}
		
		
		private function onStageMouseDown(ev : MouseEvent) : void
		{
			_prevMouseX = ev.stageX;
			_prevMouseY = ev.stageY;
		}
		
		private function onStageMouseMove(ev : MouseEvent) : void
		{
			if (ev.buttonDown) {
				_hoverCtrl.panAngle += (ev.stageX - _prevMouseX);
				_hoverCtrl.tiltAngle += (ev.stageY - _prevMouseY);
			}			
			_prevMouseX = ev.stageX;
			_prevMouseY = ev.stageY;
		}
		
		private function onStageMouseWheel(ev : MouseEvent) : void
		{
			_hoverCtrl.distance -= ev.delta * 5;
			
			if (_hoverCtrl.distance < 5)
				_hoverCtrl.distance = 5;
			else if (_hoverCtrl.distance > 6000)
				_hoverCtrl.distance = 6000;
		}
		
		
		private function onStageResize(ev : Event) : void
		{
			_view.width = stage.stageWidth;
			_view.height = stage.stageHeight;
		}
		
		
		private function onEnterFrame(ev : Event) : void
		{
			_hoverCtrl.update();
						
			_view.render();
		}
	}
}