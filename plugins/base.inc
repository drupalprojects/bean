<?php

/**
 * @file
 * Base Plugin Class
 */

abstract class bean_plugin {
  protected $plugin_info;

  /**
   * Get Plugin info
   */
  public function getInfo($key = NULL) {
    if (!empty($key) && isset($this->plugin_info[$key])) {
      return $this->plugin_info[$key];
    }

    return $this->plugin_info;
  }

  public function __construct($plugin_info) {
    $this->plugin_info = $plugin_info;
  }

  /**
   * Add a Bean to the plugin
   */
  public function setBean(Bean $bean) {
    $this->bean = $bean;
  }

  /**
   * Define the form values
   */
  abstract public function values();

  /**
   * The Plugin Form
   */
  abstract protected function form($bean);

  /**
   * THe plugin submit function
   */
  abstract protected function validate($values);

}