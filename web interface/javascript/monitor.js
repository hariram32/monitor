function refresh_grid( )
{
	//window.alert("Refreshing");
	var xhttp = new XMLHttpRequest( );
	xhttp.onreadystatechange = function( )
	{
		if ( xhttp.readyState == 4 && xhttp.status == 200 ) 
		{
			//window.alert("Ready state: " + xhttp.readyState);
			document.getElementById( "grid" ).innerHTML = xhttp.responseText;
			$(function () { $("[data-toggle='popover']").popover(); });
			/*$( document ).ready(function() {
				$("#grid_table").on("mouseenter", "td",
				function() {
				  $("[rel='popover']").popover();
				  console.log("MOUSEENTER");
				}).on("mouseleave", "td",
				function() {
				  //$("[rel='tooltip']").tooltip();
				  console.log("MOUSELEAVE");
				});	
			} );*/				
			/*$("#grid_table").on("mouseenter", "td",
			function() {
			  $(this).children("div").css('display', 'block');
			  console.log("MOUSEENTER");
			}).on("mouseleave", "td",
			function() {
			  $(this).children("div").css('display', 'none');
			  console.log("MOUSELEAVE");
			});*/
		}
		else
		{
			//window.alert("Ready state: " + xhttp.readyState);
		}
	};
	xhttp.open("GET", "php/grid.php", true);
	xhttp.send();
}
